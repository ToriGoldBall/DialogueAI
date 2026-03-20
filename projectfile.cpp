#include "projectfile.h"

#include <QRegularExpression>
#include <iostream>
#include <fstream>

ProjectFile::ProjectFile()
{
    //create a dummy file structure
    m_ProjectFile = std::make_shared<DialAi::files::ProjectNode>();
}

ProjectFile::~ProjectFile()
{

}

std::optional<std::string> ProjectFile::CreateNewProject(std::filesystem::path targetPath)
{
    //first define the project directories
    std::filesystem::path chars = targetPath;
    std::filesystem::path installs = targetPath;
    chars /= DialAi::files::charactersDirName;
    installs /= DialAi::files::installmentsDirName;

    //then try to create each one (unless they already exists)
    if(!std::filesystem::exists(targetPath) && !std::filesystem::create_directories(targetPath))
    {
        return "Failed to Create Directory " + targetPath.string();
    }
    if(!std::filesystem::exists(chars) && !std::filesystem::create_directories(chars))
    {
        return "Failed to Create Directory " + chars.string();
    }
    if(!std::filesystem::exists(installs) && !std::filesystem::create_directories(installs))
    {
        return "Failed to Create Directory " + chars.string();
    }
    //everything is successful, go ahead and set the open project
    m_ProjectFile->OpenProject(targetPath);

    UpdateRecentProjectList();

    return std::nullopt;
}

bool ProjectFile::ProjectExistsAt(std::filesystem::path targetDir)
{
    return DialAi::files::ProjectNode::HasProjectAtPath(targetDir);
}

bool ProjectFile::IsProjectOpen()
{
    return !m_ProjectFile->GetProjectDir().empty();
}

std::optional<std::string> ProjectFile::SaveProject()
{
    //TODO: can save fail? files locked maybe?
    if(m_ProjectFile->HasUnsavedChanges())
    {
        return m_ProjectFile->SaveChangesRecursive();
    }
    return std::nullopt;
}

std::optional<std::string> ProjectFile::ExportProject()
{
    //Create a CSV file in the main directory containing the diagnostics
    std::filesystem::path report = m_ProjectFile->GetProjectDir();
    report += "report.csv";
    //if the report exists, delete it
    if(std::filesystem::exists(report))
    {
        if(!std::filesystem::remove(report))
        {
            return "Unable to clear out old report.csv file";
        }
    }
    //create a new report
    std::ofstream fs(report, std::fstream::out);
    fs <<"A,B" << std::endl;
    fs.close();

    return std::nullopt;
}

std::optional<std::string> ProjectFile::OpenProject(std::filesystem::path projPath)
{
    try
    {
        if(m_ProjectFile->OpenProject(projPath))
        {
            UpdateRecentProjectList();
            return std::nullopt;
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "DialogueAI: Filesystem error " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "DialogueAI: Filesystem error when scanning project path: " << projPath << std::endl;
    }

    return "Open Project Failed";
}

std::pair<std::string,std::filesystem::path> ProjectFile::GetCurrentProject()
{
    auto projectPath = m_ProjectFile->GetProjectDir();
    return {projectPath.filename(), projectPath};
}

std::optional<std::string> ProjectFile::CloseProject()
{
    //clear the project
    m_ProjectFile->CloseProject();
    //TODO: check for unsaved changes
    return std::nullopt;
}

std::vector<std::pair<std::string, std::filesystem::path>> ProjectFile::GetRecentProjectList()
{
    std::vector<std::pair<std::string, std::filesystem::path>> recentFiles;
    QStringList list = m_LoadedSettings.value("RecentProjects", "").toString().split(QRegularExpression("[,]"), Qt::SkipEmptyParts);
    for(auto item : list)
    {
        std::filesystem::path path(item.toStdString());
        //only include if it's a directory
        if(std::filesystem::is_directory(path))
        {
            recentFiles.emplace_back(std::pair<std::string, std::filesystem::path>(path.filename(), path));
        }
    }
    return recentFiles;
}

bool ProjectFile::HasUnsavedChanges()
{
    return IsProjectOpen() && m_ProjectFile->HasUnsavedChanges();
}


void ProjectFile::UpdateRecentProjectList()
{
    //prepare new recent
    const std::string newRecent = m_ProjectFile->GetProjectDir();

    //get current recent list
    const std::vector<std::pair<std::string, std::filesystem::path>>& recentList = GetRecentProjectList();

    //we'll just keep things simple and assume comma separated paths
    std::string newRecents = newRecent;
    for(int i=0; i<recentList.size() && i < 10; i++)
    {
        if(recentList[i].second.string().compare(newRecent) != 0)
        {
            newRecents += "," + recentList[i].second.string();
        }
    }

    //set the key
    QString recents{newRecents.c_str()};
    m_LoadedSettings.setValue("RecentProjects", recents);
}

std::vector<std::shared_ptr<DialAi::files::Character>> ProjectFile::GetCharacters()
{
    if(IsProjectOpen())
    {
        return m_ProjectFile->getCharacters();
    }
    return {};
}

std::optional<std::string> ProjectFile::AddNewCharacter(std::string charName, std::string bio, std::string srcImage)
{
    DialAi::files::Character c(m_ProjectFile);
    c.setCharacterName(charName);
    c.setCharacterDesc(bio);
    c.setCharacterImageSource(srcImage);

    return m_ProjectFile->AddCharacter(c);
}

std::vector<std::shared_ptr<DialAi::files::Installment>> ProjectFile::GetInstallments()
{
    if(IsProjectOpen())
    {
        return m_ProjectFile->getInstallments();
    }
    return {};
}

//creates a new character directory
std::optional<std::string> ProjectFile::AddNewInstallment(std::string installmentTitle, std::string description, std::string srcImage)
{
    DialAi::files::Installment i(m_ProjectFile);
    i.setInstallmentTitle(installmentTitle);
    i.setInstallmentDesc(description);
    i.setInstallmentImage(srcImage);

    return m_ProjectFile->AddInstallment(i);
}
