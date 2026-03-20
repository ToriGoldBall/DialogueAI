#ifndef DIALAIFILEWRITER_H
#define DIALAIFILEWRITER_H

#endif // DIALAIFILEWRITER_H

#include <vector>
#include <fstream>
#include <filesystem>
#include <memory>
#include <optional>
#include <algorithm>

//namespace to keep things from conflicting with other libs
namespace DialAi
{
    namespace files
    {
        //mp3 extension
        const std::string mp3Extension = ".mp3";
        //text extension
        const std::string txtExtension = ".txt";
        //image extension
        const std::string pngExtension = ".png";

        //Project's tempFile Prepend
        const std::string projTempPrepend = "DialAi_Temp";
        //Project's undoFile (in system temp)
        const std::string projUndoFile = "DialAi_UndoFile.txt";

        //Text to prepend onto the front of images used by the project
        const std::string imageNamePrepend = "DialAi_Image";
        //Text to prepend onto the fron of description texts
        const std::string descriptionTextPrepend = "DialAi_Text";
        //The name for a sample
        const std::string audioFilePrepend = "DialAi_Sound";

        //the directory name of the Characters directory
        const std::string charactersDirName = "Characters";
        //the directory name of the Installments directory
        const std::string installmentsDirName = "Installments";

        //a default value in case things get messed up
        const std::string unknownChracter = "unknown";

        const int maxTxtFileRead = 500;

        //Reads a text file indicated by the filePath and returns the contents as a string provided it is a DialAI file
        static std::string ReadTextFile(std::filesystem::path filePath)
        {
            std::string fileContents(maxTxtFileRead, '\0');
            if(filePath.extension() == txtExtension && filePath.filename().string().starts_with(descriptionTextPrepend))
            {
                std::ifstream file(filePath, std::ios::in);
                if(file && file.is_open())
                {
                    file.read(&fileContents[0], fileContents.size());
                    fileContents.resize(file.gcount());
                }
            }
            return fileContents;
        }
        //creates a txt file with the given name at the given path with the provided contents
        static std::optional<std::string> WriteTextFile(std::filesystem::path path, std::string fileName, std::string contents)
        {
            std::optional<std::string> errMessage = std::nullopt;

            if(std::filesystem::exists(path) && !fileName.empty())
            {
                auto&& newPath = path / fileName / txtExtension;
                std::ofstream outputFile(newPath, std::ios_base::trunc);
                if(outputFile.is_open())
                {
                    outputFile << contents;
                }
                else
                {
                    errMessage = "Unable to open " + newPath.string() + " for writing.";
                }
                outputFile.close();
            }
            else
            {
                errMessage = "Name is empty or " + path.string() + " does not exist.";
            }
            return errMessage;
        }
        //Overwrites a file at the destination path with the source path
        static std::optional<std::string> OverwriteFile(std::filesystem::path& sourcePath, const std::filesystem::path destPath)
        {
            if(sourcePath == destPath)
            {
                return std::nullopt;
            }
            if(std::filesystem::exists(sourcePath))
            {
                //if the old file exists, delete it
                if(std::filesystem::exists(destPath))
                {
                    if(!std::filesystem::remove(destPath))
                    {
                        return "Cannot remove current file:\n" + destPath.string();
                    }
                }
                //try to copy the source to the destination
                std::filesystem::copy(sourcePath, destPath);
                if(!std::filesystem::exists(destPath))
                {
                    return "Unable to copy: " + sourcePath.string();
                }
            }
            //overwrite the path
            sourcePath = destPath;
            return std::nullopt;
        }


        //Base Character node for type safetypath
        class BaseCharacterNode
        {
        public:
            //the name of the character (from directory)
            virtual std::string getCharacterName() = 0;
            virtual std::optional<std::string> setCharacterName(std::string name) = 0;
            //the description of the character (from text file)
            virtual std::string getCharacterDesc() = 0;
            virtual std::optional<std::string> setCharacterDesc(std::string desc) = 0;
            //the main image for this character DialAi_Image_{}.png
            virtual std::string getCharacterImageSource()     = 0;
            virtual std::optional<std::string> setCharacterImageSource(std::string fileName)     = 0;
        };

        class FileNodeBase : public std::enable_shared_from_this<FileNodeBase>
        {
        public:
            virtual void Read(const std::filesystem::path& dirToScan) = 0;
            virtual std::optional<std::string> Write() = 0;
            virtual std::filesystem::path getPath() = 0;
            virtual std::shared_ptr<FileNodeBase> GetParent(const std::string& targetType) = 0;
        };

        //base class for the other objects to work off of providing the directory read and tree interactions
        template<typename T>
        class FileNode : public FileNodeBase
        {
        public:
            FileNode(std::shared_ptr<FileNodeBase> parent)
                : m_Parent(parent), m_IsDirty(false)
            {
            };

            //reads the path into this node
            virtual void Read(const std::filesystem::path& dirToScan) override
            {
                //create a pointer to share with the children for path generation
                std::shared_ptr<FileNodeBase> thisShrPtr = shared_from_this();

                m_directoryName = dirToScan.parent_path().filename();
                //iterate over all of this directory's entries
                for(auto&& dirEntry : std::filesystem::directory_iterator(dirToScan))
                {
                    std::filesystem::path dirEntryPath = dirEntry.path();
                    if (dirEntryPath.has_extension())
                    {
                        std::string ext = dirEntryPath.extension();
                        if(ext == pngExtension && dirEntryPath.filename().string().starts_with(imageNamePrepend))
                        {
                            m_imageSourcePath = dirEntryPath;
                        }
                        else if(ext == txtExtension && dirEntryPath.filename().string().starts_with(descriptionTextPrepend))
                        {
                            m_textFileName = dirEntryPath.filename();
                            m_textFileContents = ReadTextFile(dirEntryPath);
                        }
                        else if(ext == mp3Extension && dirEntryPath.filename().string().starts_with(audioFilePrepend))
                        {
                            m_soundSourcePath = ReadTextFile(dirEntryPath);
                        }
                    }
                    else if(dirEntry.is_directory())
                    {
                        std::shared_ptr<T> childDir = std::make_shared<T>(thisShrPtr);
                        childDir->Read(dirEntryPath);
                        children.emplace_back(childDir);
                    }
                }
            }
            //writes this node to its path
            virtual std::optional<std::string> Write() override
            {
                std::optional<std::string> error = std::nullopt;
                //if no changes do nothing
                if(!m_IsDirty)
                {
                    return error;
                }
                std::vector<std::optional<std::string>> errors;
                //start out with trying to make the directory
                if(!m_directoryName.empty())
                {
                    auto path = getPath() / m_directoryName;
                    //if the directory does not already exist and we fail to create it
                    if(!std::filesystem::exists(path) && !std::filesystem::create_directory(path))
                    {
                        errors.emplace_back("Directory for " + m_directoryName + " could not be created.");
                    }
                    else
                    {
                        //write out the text file if we have contents
                        if(!m_textFileContents.empty())
                        {
                            errors.emplace_back(WriteTextFile(path, m_textFileName, m_textFileContents));
                        }
                        //if an image source path is defined then attemp an overwrite
                        if(!m_imageSourcePath.empty())
                        {
                            auto err = OverwriteFile(m_imageSourcePath, path / (imageNamePrepend + pngExtension));
                            if(err.has_value())
                            {
                                errors.emplace_back(err);
                            }
                        }
                        //if an mp3 source path is defined then attemp an overwrite
                        if(!m_soundSourcePath.empty())
                        {
                            auto err = OverwriteFile(m_soundSourcePath, path / (audioFilePrepend + mp3Extension));
                            if(err.has_value())
                            {
                                errors.emplace_back(err);
                            }
                        }
                    }
                }
                else
                {
                    errors.emplace_back("Name is empty");
                }

                for(auto err : errors)
                {
                    if(err != std::nullopt)
                    {
                        error = ((error != std::nullopt) ? error.value() + "\n" : "") + err.value();
                    }
                }
                return error;
            }

            //retrieves the path from this node, up to the top node
            virtual std::filesystem::path getPath() override
            {
                if(m_Parent)
                {
                    auto path = m_Parent->getPath();
                    path += m_directoryName;
                    return path;
                }
                return m_directoryName;
            }

            //recursively search for any dirty children
            virtual bool HasUnsavedChanges()
            {
                if(m_IsDirty)
                {
                    return true;
                }
                for(auto child : getChildren())
                {
                    if(child->HasUnsavedChanges())
                    {
                        return true;
                    }
                }
                return false;
            }
            //recursively save any changes to a specific node and all children
            virtual std::optional<std::string> SaveChangesRecursive()
            {
                if(m_IsDirty)
                {
                    //try to apply the changes for any temp files
                    if(auto error = Write(); error != std::nullopt)
                    {
                        return error;
                    }
                    m_IsDirty = false;
                }
                for(auto childNode : getChildren())
                {
                    if(auto error = childNode->SaveChangesRecursive(); error != std::nullopt)
                    {
                        return error;
                    }
                }
                return std::nullopt;
            }
            //undo changes made to file (via temp caching)
            virtual bool UndoChanges()
            {
                //TODO: utilitize the temp files with node paths
                return false;
            }

            //searches, and returns a pointer to, all children that have changes
            virtual std::vector<std::shared_ptr<T>> getChildrenWithChanges()
            {
                std::vector<std::shared_ptr<T>> children;
                for(auto childNode : getChildren())
                {
                    if(childNode->HasUnsavedChanges())
                    {
                        children.emplace_back(childNode);
                    }
                }
                return children;
            }
            //getter for the children of this fileNode
            virtual std::vector<std::shared_ptr<T>> getChildren()
            {
                return children;
            }

            //adds a child to this node's list of children
            virtual std::optional<std::string> addChild(std::shared_ptr<T> child)
            {
                std::optional<std::string> message = std::nullopt;

                //ensure there is no duplicate childName
                std::string childName = std::filesystem::path(child->getPath()).filename();
                std::transform(std::begin(childName), std::end(childName), std::begin(childName), [](char c) {return std::tolower(c);});
                for(std::shared_ptr<T> c : children)
                {
                    std::string curChildName = c->getDirName();
                    std::transform(std::begin(curChildName), std::end(curChildName), std::begin(curChildName), [](char c) {return std::tolower(c);});
                    if(childName.compare(curChildName) == 0)
                    {
                        message = childName;
                    }
                }
                //if no error being reported, add the child
                if(message == std::nullopt)
                {
                    children.emplace_back(child);
                }

                return message;
            }

            //getter for this node's parent
            virtual std::shared_ptr<FileNodeBase> GetParent(const std::string& targetTypeIdName) override
            {
                if(m_Parent != nullptr)
                {
                    std::string typeName = typeid(m_Parent).name();
                    if(typeName.compare(targetTypeIdName) == 0)
                    {
                        return m_Parent;
                    }
                    return m_Parent->GetParent(targetTypeIdName);
                }
                return nullptr;
            }

            std::string getDirName() const { return getNodeName(); }

        protected:
            //Getters
            virtual std::string getNodeName() const { return m_directoryName; }
            virtual std::string getTextFileContents() const { return m_textFileContents; }
            virtual std::string getImageSource() const { return m_imageSourcePath; }
            virtual std::string getSoundSource() const { return m_soundSourcePath; }

            //Setters so we can track dirty status
            virtual std::optional<std::string> setNodeName(std::string newName)
            {
                if(newName.empty()) { return "Name cannot be empty"; }
                m_IsDirty = true;
                m_directoryName = newName;
                return std::nullopt;
            }
            virtual std::optional<std::string> setTextFileContents(std::string contents)
            {
                m_IsDirty = true;
                m_textFileContents = contents;
                return std::nullopt;
            }
            virtual std::optional<std::string> setImageSource(std::string newSource)
            {
                if(!newSource.empty())
                {
                    if(!std::filesystem::exists(newSource))
                    {
                        return "Source path does not exist";
                    }
                    std::filesystem::path p(newSource);
                    if(!p.has_extension() || p.extension() != pngExtension)
                    {
                        return "Source is not a png file";
                    }
                    m_IsDirty = true;
                    m_imageSourcePath = p;
                }
                else
                {
                    m_imageSourcePath.clear();
                }
                return std::nullopt;
            }
            virtual std::optional<std::string> setSoundSource(std::string newSource)
            {
                if(!newSource.empty())
                {
                    if(!std::filesystem::exists(newSource))
                    {
                        return "Source path does not exist";
                    }
                    std::filesystem::path p(newSource);
                    if(!p.has_extension() || p.extension() != mp3Extension)
                    {
                        return "Source is not an mp3 file";
                    }
                    m_IsDirty = true;
                    m_soundSourcePath = newSource;
                }
                else
                {
                    m_soundSourcePath.clear();
                }
                return std::nullopt;
            }

        private:
            std::shared_ptr<FileNodeBase> m_Parent; //the parent of this node

            std::string             m_directoryName;    //the name of the FileNode (from directory)
            std::string             m_textFileName;     //a text file describing contents DialAi_Text_{}.txt
            std::string             m_textFileContents; //the contents of the text file

            std::filesystem::path   m_imageSourcePath;  //the full path to an image for the sample DialAi_Image_{}.png
            std::filesystem::path   m_soundSourcePath;  //the full path to a sound file DialAi_Sound_{}.mp3


            bool m_IsDirty;                 //whether or not this has unsaved changes

            std::vector<std::shared_ptr<T>> children;        //child directories/nodes (if supported)
        };

        //a terminating type for the tree
        class LeafNode : public FileNode<LeafNode>
        {
        public:
            LeafNode(std::shared_ptr<FileNodeBase> parent) : FileNode(parent){}

            void Read(const std::filesystem::path& dirToScan) override {}
            std::optional<std::string> Write() override { return std::nullopt; }

            std::filesystem::path getPath() override { return ""; }

            //recursively search for any dirty children
            bool HasUnsavedChanges() override {return false;}
            //recursively save any changes to both sets of children
            std::optional<std::string> SaveChangesRecursive() override {return std::nullopt;}
            //undo changes made to file (via temp caching)
            bool UndoChanges() override {return false;}

            //searches, and returns a pointer to, all children that have changes
            std::vector<std::shared_ptr<LeafNode>> getChildrenWithChanges() override {return {};}

            virtual std::vector<std::shared_ptr<LeafNode>> getChildren() override { return {}; };
            virtual std::optional<std::string> addChild(std::shared_ptr<LeafNode> child) override { return std::nullopt; }

            virtual std::string getNodeName() const  override {return "";}
            virtual std::string getTextFileContents() const  override {return "";}
            virtual std::string getImageSource() const  override {return "";}
            virtual std::string getSoundSource() const  override {return "";}
            virtual std::optional<std::string> setNodeName(std::string newName) override {return std::nullopt;}
            virtual std::optional<std::string> setTextFileContents(std::string contents) override {return std::nullopt;}
            virtual std::optional<std::string> setImageSource(std::string newSource) override {return std::nullopt;}
            virtual std::optional<std::string> setSoundSource(std::string newSource) override {return std::nullopt;}
        };

        //a struct for storing the available Voice Samples of a Character
        //leaf node presented for easing type termination
        class ActorSample : public FileNode<LeafNode>
        {
        public:
            ActorSample(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //the name of the sample (from directory), happy, yelling, etc.
            const std::string sampleName(){ return getNodeName();}
            std::optional<std::string> setSampleName(std::string dirName) { return setNodeName(dirName);}

            //the image of the character relevant to the sample
            std::string getSampleImageSource() { return getImageSource(); }
            std::optional<std::string> setSampleImageSource(std::string filePath) { return setImageSource(filePath); }

            //the filename for this sample DilaAi_Sample_{}.mp3
            std::string getSampleSource()  { return getSoundSource(); }
            std::optional<std::string> setSampleSource(std::string filePath)  { return setSoundSource(filePath); }
        };

        //Character class
        class Character : public FileNode<ActorSample>, public BaseCharacterNode
        {
        public:
            Character(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //the name of the character (from directory)
            virtual std::string getCharacterName() override { return getNodeName(); }
            virtual std::optional<std::string> setCharacterName(std::string name) override { return setNodeName(name); }

            //the description of the character (from text file)
            virtual std::string getCharacterDesc() override { return getTextFileContents(); }
            virtual std::optional<std::string> setCharacterDesc(std::string desc) override { return setTextFileContents(desc); }

            //the main image for this character DialAi_Image_{}.png
            virtual std::string getCharacterImageSource() override { return getImageSource(); }
            virtual std::optional<std::string> setCharacterImageSource(std::string fileName) override { return setImageSource(fileName); }

            //different samples (happy, mad, excited, yelling, etc.) as folders
            std::vector<std::shared_ptr<ActorSample>> samples() { return getChildren(); }
        };

        //The line read class, either representing a sample or an override, as a numeric folder
        class LineRead : public FileNode<LeafNode>
        {
        public:
            LineRead(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //A sample-output file pair available for a line read
            //For the AI, if no voiceSample available, do nothing, this is an override
            //For override, don't write a voiceSample, just write the output

            //the input sample path (from text file)
            std::string selectedSample()    { return getTextFileContents(); }
            //the actual output, filename DialAi_Output.mp3
            std::string output()            { return getSoundSource(); }
        };

        //The spoken line class, having a line to read, a character, and the possible line reads as folders
        class SpokenLine : public FileNode<LineRead>
        {
        public:
            SpokenLine(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //override read so that we can get the character we are associated with
            virtual void Read(const std::filesystem::path& dirToScan) override;

            //the line the character reads (from text file)
            std::string getLine()               { return getTextFileContents(); }
            //the character that reads it (from numeric folder name)
            std::string getCharacterReading(bool keepUnderscores = false);

            //the POSSIBLE line reads as numeric folders
            std::vector<std::shared_ptr<LineRead>> lineReads() { return getChildren(); }

        private:
            std::shared_ptr<BaseCharacterNode> m_Character;   //the retrieved character name from the folder name

            //Note: 00_ prepends a duplicated, and empty, folder matching the current selection
        };

        //A scene within an Act
        class Scene : public FileNode<SpokenLine>
        {
        public:
            Scene(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //title of the scene (from folder)
            std::string getSceneTitle()    { return getNodeName(); }
            //description of the scene (from text file)
            std::string getSceneDesc()     { return getTextFileContents(); }
            //the image for this scene
            std::string getSceneImage()     { return getImageSource(); }

            //the spoken lines in the scene as numeric folders (for sequence)
            std::vector<std::shared_ptr<SpokenLine>> getLines() { return getChildren(); }
        };

        //An act within an installment
        class Act : public FileNode<Scene>
        {
        public:
            Act(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //title of the act (from folder)
            std::string getActTitle()      { return getNodeName(); }
            //description of the act (from text file)
            std::string getActDesc()       { return getTextFileContents(); }
            //the image for this act
            std::string getActImage()     { return getImageSource(); }

            //the separate scenes (as alpha & numeric folders)
            std::vector<std::shared_ptr<Scene>> actScenes() { return getChildren(); }
        };

        //The class for representing an installment (episode, level, etc.)
        class Installment : public FileNode<Act>
        {
        public:
            Installment(std::shared_ptr<FileNodeBase> parent) : FileNode(parent) {}

            //title of the installment (from folder)
            virtual std::string getInstallmentTitle()   { return getNodeName(); }
            virtual std::optional<std::string> setInstallmentTitle(std::string title)   { return setNodeName(title); }
            //description of the installment (from text file)
            virtual std::string getInstallmentDesc()    { return getTextFileContents(); }
            virtual std::optional<std::string> setInstallmentDesc(std::string desc)     { return setTextFileContents(desc); }
            //the image for this installment
            virtual std::string getInstallmentImage()   { return getImageSource(); }
            virtual std::optional<std::string> setInstallmentImage(std::string imagePath) { return setImageSource(imagePath); }

            //the separate acts (as alpha & numeric folders)
            std::vector<std::shared_ptr<Act>> installmentActs() { return getChildren(); }
        };

        //the main project node containing all characters and installments
        class ProjectNode : public FileNode<LeafNode>
        {
        public:
            ProjectNode() : FileNode(nullptr) {};
            virtual ~ProjectNode() {};

            //attempts to open a project located at a specified path
            bool OpenProject(const std::filesystem::path& candidatePath);

            //clears the directory and deletes the tree
            void CloseProject();

            //Get the current project directory, if one is set
            std::filesystem::path GetProjectDir() const { return m_ProjectDir; }

            //checks if the given path contains a valid project
            static bool HasProjectAtPath(const std::filesystem::path& candidatePath);

            //whether or not there are unsaved changes in the project tree
            bool HasUnsavedChanges() override;
            //apply all project changes to directories
            std::optional<std::string> SaveChangesRecursive() override;
            //undo changes made to file (via temp caching)
            bool UndoChanges() override;

            //getter for the characters in the project
            virtual std::vector<std::shared_ptr<Character>>& getCharacters();
            //attempts to add the provided character, returning an error message otherwise
            std::optional<std::string> AddCharacter(Character c);

            //getter for the installments in the project
            virtual std::vector<std::shared_ptr<Installment>>& getInstallments();
            //attempts to add the provided installment, returning an error message otherwise
            std::optional<std::string> AddInstallment(Installment i);

            ////reads the path into this node
            virtual void Read(const std::filesystem::path& dirToScan) override {}
            ////retrieves the path from this node, up to the top node
            virtual std::filesystem::path getPath() override { return m_ProjectDir; }

            //override the irrelevant getters
            virtual std::vector<std::shared_ptr<LeafNode>> getChildrenWithChanges() override { return {}; }
            virtual std::vector<std::shared_ptr<LeafNode>> getChildren() override { return {}; }
            virtual std::optional<std::string> addChild(std::shared_ptr<LeafNode> child) override {return std::nullopt;};
            virtual std::shared_ptr<FileNodeBase> GetParent(const std::string& targetTypeIdName) override { return nullptr; }

        private:
            //project's directory
            std::filesystem::path m_ProjectDir;

            //the collection of characters in this project
            std::vector<std::shared_ptr<Character>> m_ProjectCharacters;
            //the collection of installments in this project
            std::vector<std::shared_ptr<Installment>> m_ProjectInstallments;
        };
    }
}
