#include "DialAiFileWriter.h"

#include <algorithm>
#include <typeinfo>

using namespace DialAi::files;

void SpokenLine::Read(const std::filesystem::path& dirToScan)
{
    //call the base version
    FileNode::Read(dirToScan);
    //grab the character's name indicated by the directory
    auto charName = getCharacterReading(true);

    //find the project node and get the characters from it

    if(auto&& node = GetParent(typeid(ProjectNode).name()); node)
    {
        if(auto&& project = std::dynamic_pointer_cast<ProjectNode>(node); project)
        {
            //then work out which character we refer to (if we can)
            for(auto&& character : project->getCharacters())
            {
                auto name = character->getCharacterName();
                if(charName == name)
                {
                    m_Character = character;
                }
            }
        }
    }
}

std::string SpokenLine::getCharacterReading(bool keepUnderscores /*= false*/)
{
    if(m_Character)
    {
        //grab the directory and skip the numeric portion
        std::string dirName = getNodeName();
        std::size_t index = dirName.find_first_of('_');
        if(index != std::string::npos)
        {
            std::string name = dirName.substr(index+1);
            //for safety, spaces were replaced with underscores
            //if underscores are not desired, remove them and replace with spaces
            if(!keepUnderscores)
            {
                std::replace(name.begin(), name.end(), '_', ' ');
            }
            return name;
        }
    }
    return unknownChracter;
}

//attempts to open a project located at a specified path
bool ProjectNode::OpenProject(const std::filesystem::path& candidatePath)
{
    if(HasProjectAtPath(candidatePath))
    {
        m_ProjectDir = candidatePath;

        //create a pointer to share with the children for path generation
        std::shared_ptr<FileNodeBase> thisShrPtr = shared_from_this();

        for(auto&& dir : std::filesystem::directory_iterator(GetProjectDir()))
        {
            if(dir.is_directory() && dir.path().filename().compare("Characters") == 0)
            {
                for(auto&& childDir : std::filesystem::directory_iterator(dir))
                {
                    if(childDir.is_directory())
                    {
                        std::shared_ptr<Character> c = std::make_shared<Character>(thisShrPtr);
                        c->Read(childDir);
                        m_ProjectCharacters.emplace_back(c);
                    }
                }
            }
            else if(dir.is_directory() && dir.path().filename().compare("Installments") == 0)
            {
                for(auto&& childDir : std::filesystem::directory_iterator(dir))
                {
                    if(childDir.is_directory())
                    {
                        std::shared_ptr<Installment> i = std::make_shared<Installment>(thisShrPtr);
                        i->Read(childDir);
                        m_ProjectInstallments.emplace_back(i);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

void ProjectNode::CloseProject()
{
    m_ProjectDir.clear();
    m_ProjectCharacters.clear();
    m_ProjectInstallments.clear();
}

bool ProjectNode::HasProjectAtPath(const std::filesystem::path& candidatePath)
{
    std::filesystem::path chars = candidatePath;
    std::filesystem::path installs = candidatePath;
    chars /= DialAi::files::charactersDirName;
    installs /= DialAi::files::installmentsDirName;

    //if there are 2 directories, chracters and installments, then this looks like our kind of project
    if(std::filesystem::exists(chars) && std::filesystem::is_directory(chars) &&
            std::filesystem::exists(installs) && std::filesystem::is_directory(installs))
    {
        return true;
    }
    return false;
}

//whether or not there are unsaved changes in the project tree
bool ProjectNode::HasUnsavedChanges()
{
    //recursively check characters for unsaved changes
    for(std::shared_ptr<Character>& c : m_ProjectCharacters)
    {
        if(c->HasUnsavedChanges())
        {
            return true;
        }
    }
    //recursively check installments for unsaved changes
    for(std::shared_ptr<Installment>& c : m_ProjectInstallments)
    {
        if(c->HasUnsavedChanges())
        {
            return true;
        }
    }
    return false;
}

//apply changes to file
std::optional<std::string> ProjectNode::SaveChangesRecursive()
{
    //save the changes to the actual directories
    //recursively save characters
    for(std::shared_ptr<Character>& c : m_ProjectCharacters)
    {
        if(auto error = c->SaveChangesRecursive(); error != std::nullopt)
        {
            return error;
        }
    }
    //recursively save installments
    for(std::shared_ptr<Installment>& i : m_ProjectInstallments)
    {
        if(auto error = i->SaveChangesRecursive(); error != std::nullopt)
        {
            return error;
        }
    }
    return std::nullopt;
}


//undo changes made to file (via temp caching)
bool ProjectNode::UndoChanges()
{
    //
    return false;
}

//getter for the characters in the project
std::vector<std::shared_ptr<Character>>& ProjectNode::getCharacters()
{
    return m_ProjectCharacters;
}

std::optional<std::string> ProjectNode::AddCharacter(Character c)
{
    for(auto&& m_c : m_ProjectCharacters)
    {
        if (m_c->getCharacterName() == c.getCharacterName())
        {
            return "A character by that name already exists";
        }
    }
    m_ProjectCharacters.emplace_back(std::make_shared<Character>(c));

    return std::nullopt;
}

//getter for the Installments in the project
std::vector<std::shared_ptr<Installment>>& ProjectNode::getInstallments()
{
    return m_ProjectInstallments;
}

std::optional<std::string> ProjectNode::AddInstallment(Installment i)
{
    for(auto&& m_i : m_ProjectInstallments)
    {
        if (m_i->getInstallmentTitle() == i.getInstallmentTitle())
        {
            return "An Installment by that name already exists";
        }
    }
    m_ProjectInstallments.emplace_back(std::make_shared<Installment>(i));

    return std::nullopt;
}
