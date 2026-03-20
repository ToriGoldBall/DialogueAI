#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QSettings>

#include "DialAiFileWriter.h"

#include <filesystem>

//a class for creating and interacting with project files
class ProjectFile
{
public:
    ProjectFile();
    ~ProjectFile();

    //Create a new project in the target directory, returns the error message if failed
    std::optional<std::string> CreateNewProject(std::filesystem::path targetPath);

    //check if the given dir contains the necessary project directories
    bool ProjectExistsAt(std::filesystem::path targetDir);

    //Checks if a project is open
    bool IsProjectOpen();

    //Save the Current Project
    std::optional<std::string> SaveProject();

    //Export the Project
    std::optional<std::string> ExportProject();

    //Open project, returns the error message if failed
    virtual std::optional<std::string> OpenProject(std::filesystem::path projPath);

    //Retrieves the name of the current project (directory)
    std::pair<std::string,std::filesystem::path> GetCurrentProject();

    //Closes the current project
    std::optional<std::string> CloseProject();

    //Acquire the recent projects using QT's configuration settings (override if not using QT backend)
    virtual std::vector<std::pair<std::string, std::filesystem::path>> GetRecentProjectList();

    //checks if the current project file has any unsaved changes
    bool HasUnsavedChanges();

    //gets the list of characters
    std::vector<std::shared_ptr<DialAi::files::Character>> GetCharacters();
    //creates a new character directory
    std::optional<std::string> AddNewCharacter(std::string charName, std::string bio, std::string srcImage);

    //gets the list of characters
    std::vector<std::shared_ptr<DialAi::files::Installment>> GetInstallments();
    //creates a new character directory
    std::optional<std::string> AddNewInstallment(std::string installmentTitle, std::string description, std::string srcImage);

private:
    //Updates the recent project list by prepending the current open project
    virtual void UpdateRecentProjectList();

private:
    //The app's configuration settings
    QSettings m_LoadedSettings;

    //the Project file/directory tree
    std::shared_ptr<DialAi::files::ProjectNode> m_ProjectFile;
};

#endif // PROJECTFILE_H
