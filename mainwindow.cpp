#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QFileDialog>
#include <QLineEdit>

#include "Dialogs/addcharacterdialog.h"

namespace
{
    void enableForProject(Ui::MainWindow* ui, ProjectFile* proj)
    {
        bool haveProject = proj->IsProjectOpen();
        ui->AddCharacter->setEnabled(haveProject);
        ui->AddInstallment->setEnabled(haveProject);
        ui->actionClose->setEnabled(haveProject);
        ui->actionExport->setEnabled(haveProject);
        ui->actionSave->setEnabled(haveProject);         //this isn't great, but we don't have to convolute for it
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_project(std::make_unique<ProjectFile>())
    , m_characterList(std::make_unique<QStandardItemModel>())
    , m_installmentList(std::make_unique<QStandardItemModel>())
{
    ui->setupUi(this);

    //make sure the recent list gets populated
    UpdateRecentList();

    ui->listViewCharacters->setModel(m_characterList.get());
    ui->listViewInstallments->setModel(m_installmentList.get());

    enableForProject(ui, m_project.get());
}

MainWindow::~MainWindow()
{
    delete ui;
}

//File Menu region
#pragma {
//Show recent Projects for quick opening
void MainWindow::on_actionRecent_triggered()
{
}
void MainWindow::UpdateCurrentProject()
{
    if(m_project->IsProjectOpen())
    {
        std::string projectLabel = "DialogueAI - " + m_project->GetCurrentProject().first;
        if(m_project->HasUnsavedChanges())
        {
            projectLabel += "*";
        }
        this->setWindowTitle(projectLabel.c_str());

        enableForProject(ui, m_project.get());
        UpdateCharacterList();
        UpdateInstallmentList();
    }
    else
    {
        this->setWindowTitle("DialogueAI");
    }
}

void MainWindow::UpdateRecentList()
{
    //ensure we clear any previous group
    ui->actionRecent->setMenu(nullptr);

    //make a new action
    QMenu* recentProjects = new QMenu("Recent", this);
    ui->actionRecent->setMenu(recentProjects);

    //read in the recents and parse them into a list of options
    auto recentList = m_project->GetRecentProjectList();
    for(int i=0; i<recentList.size(); i++)
    {
        QString name = QString::fromStdString(recentList[i].first);
        std::filesystem::path path = recentList[i].second;
        QAction* newAction = recentProjects->addAction(name);
        connect(newAction, &QAction::triggered, this,
                [this, path]()
                {
                    m_project->OpenProject(path);
                    this->UpdateCurrentProject();
                    this->UpdateRecentList();
                }
        );
    }
}

//Asks the user if a new project should be made, triggering the project creation attempt
void MainWindow::QueryMakeNewProject()
{
    auto decision = QMessageBox::information(this, "Alert", "There is no project open.\nMake one?", QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
    if(decision == QMessageBox::StandardButton::Ok)
    {
        on_actionNew_triggered();
    }
}

//Create New Project
void MainWindow::on_actionNew_triggered()
{
    //grab the directory
    QString directory = QFileDialog::getExistingDirectory(nullptr, "Select a location for your project", QDir::homePath());
    if(directory.isEmpty())
    {
        return;
    }
    //if a project already exists there, cancel
    if(m_project->ProjectExistsAt({directory.toStdString()}))
    {
        QMessageBox::warning(nullptr, "Cancelled", "A project already exists there.");
        auto decision = QMessageBox::information(this, "Alert", "A project already exists there.\nOpen it?", QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
        if(decision == QMessageBox::StandardButton::Yes)
        {
            OpenExistingProject(directory);
        }
        return;
    }
    //Project Name
    bool ok;
    QString projectName = QInputDialog::getText(nullptr, "Project Name", "Enter a name for your project:", QLineEdit::Normal, "", &ok);
    if(!ok)
    {
        QMessageBox::warning(nullptr, "Cancelled", "No project was created.");
        return;
    }
    //setup the new path and make the project base directories
    if(MakeNewProject(directory + "/" + projectName))
    {
        UpdateCurrentProject();
        UpdateRecentList();
    }
}
//makes a new project at the given directory
bool MainWindow::MakeNewProject(QString targetDirectory)
{
    //now try and create the new project directory
    auto errMessage = m_project->CreateNewProject(std::filesystem::path{targetDirectory.toStdString()});
    if(errMessage != std::nullopt)
    {
        QMessageBox::warning(nullptr, "Error", errMessage->c_str());
        return false;
    }
    //report success and add to the recent projects
    QMessageBox::warning(nullptr, "Success", "Created the project successfully.");
    return true;
}

//Open existing project
void MainWindow::on_actionOpen_triggered()
{
    QString directory = QDir::homePath();
    MainWindow::CompletionState projectOpened = MainWindow::CompletionState::Retry;
    while(projectOpened == MainWindow::CompletionState::Retry)
    {
        directory = QFileDialog::getExistingDirectory(this, "Select the location of your project", directory);

        //if user cancelled, there won't be a directory
        if(directory.length() == 0)
        {
            return;
        }

        //if another project is open and it has unsaved changes
        if(m_project->IsProjectOpen() && m_project->HasUnsavedChanges())
        {
            QMessageBox::warning(nullptr, "Alert", "Project AutoSaved");
            auto decision = QMessageBox::information(this, "Alert", "The current Project has unsaved changes\nSave?", QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::No);
            if(decision == QMessageBox::StandardButton::Cancel)
            {
                return;
            }
            else if(decision == QMessageBox::StandardButton::Yes)
            {
                m_project->SaveProject();
            }
        }
        projectOpened = OpenExistingProject(directory);
    }
    if(projectOpened == MainWindow::CompletionState::Success)
    {
        UpdateCurrentProject();
        UpdateRecentList();
    }
}

MainWindow::CompletionState MainWindow::OpenExistingProject(QString projectLocation)
{
    //try to open and check if there was an error
    std::optional<std::string> error = m_project->OpenProject(projectLocation.toStdString());
    if(error == std::nullopt)
    {
        QMessageBox::information(this, "Open Project Succeeded", QString("Active Directory is %1").arg(projectLocation));
        return MainWindow::CompletionState::Success;
    }
    else
    {
        auto decision = QMessageBox::information(this, "Open Project Failed", "No Project found\nDirectory 'Characters' or 'Installments' missing\nCreate One?", QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::Retry);
        if(decision == QMessageBox::StandardButton::Yes)
        {
            return MakeNewProject(projectLocation) ? MainWindow::CompletionState::Success : MainWindow::CompletionState::Failure;
        }
        else if(decision == QMessageBox::StandardButton::Cancel)
        {
            return MainWindow::CompletionState::Cancelled;
        }
        return MainWindow::CompletionState::Retry;
    }
}
//
void MainWindow::UpdateCharacterList()
{
    if(m_project->IsProjectOpen())
    {
        //bool hasItems = m_characterList->children().count();
        m_characterList->clear();
        for(auto&& character : m_project->GetCharacters())
        {
            QStandardItem* item = new QStandardItem(character->getCharacterName().c_str());
            item->setData(QVariant::fromValue(character));
            m_characterList->appendRow(item);
        }
    }
}
//
void MainWindow::UpdateInstallmentList()
{
    if(m_project->IsProjectOpen())
    {
        //bool hasItems = m_characterList->children().count();
        m_installmentList->clear();
        for(auto&& installment : m_project->GetInstallments())
        {
            QStandardItem* item = new QStandardItem(installment->getInstallmentTitle().c_str());
            item->setData(QVariant::fromValue(installment));
            m_installmentList->appendRow(item);
        }
    }
}
//Save the current project
void MainWindow::on_actionSave_triggered()
{
    if(m_project->IsProjectOpen() && m_project->HasUnsavedChanges())
    {
        //
        if(auto message = m_project->SaveProject(); message != std::nullopt)
        {
            QMessageBox::information(this, "Save Project Failed", message->c_str());
        }
    }
}
//Close the current project
void MainWindow::on_actionClose_triggered()
{
    m_project->CloseProject();

    UpdateCurrentProject();
}
//Open the export Statistics dialog
void MainWindow::on_actionExport_triggered()
{
    m_project->ExportProject();

    QMessageBox::information(this, "Save Report", QString("report.csv created"));
}
#pragma }

//Edit Menu region
#pragma {
//Undo the previous change
void MainWindow::on_actionUndo_triggered()
{
    QMessageBox::information(this, "Undo", QString("Nothing to be Done"));
}
//Redo the impending change
void MainWindow::on_actionRedo_triggered()
{
    QMessageBox::information(this, "Redo", QString("Nothing to be Done"));
}
//Open the adjust application preferences dialog
void MainWindow::on_actionPreferences_triggered()
{
    QMessageBox::information(this, "Preferences", QString("Nothing to be Done"));
}
#pragma }

//Run Menu region
#pragma {
//Runs all dialogue generators for missing or out of date lines, displaying a popup for success or fail
void MainWindow::on_actionBuild_Dialogues_triggered()
{
    QMessageBox::information(this, "Result", QString("Nothing to be Done"));
}
#pragma }

//Window Menu region
#pragma {
//Minimizes the application
void MainWindow::on_actionMinimize_triggered()
{
    if(windowState() != Qt::WindowState::WindowMinimized)
    {
        setWindowState(Qt::WindowState::WindowMinimized);
    }
}
//Maximizes the application
void MainWindow::on_actionMaximize_triggered()
{
    if(windowState() != Qt::WindowState::WindowMaximized)
    {
        setWindowState(Qt::WindowState::WindowMaximized);
    }
}
//Restores the application from Maximized
void MainWindow::on_actionRestore_triggered()
{
    if(windowState() != Qt::WindowState::WindowNoState)
    {
        setWindowState(Qt::WindowState::WindowNoState);
    }
}
#pragma }

//Help Menu region
#pragma {
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About",
                             QString("DialogueAI V0.1<br><br>Created By Tristan Aurini<br><br>Copyright 2026"));
}
#pragma }

//character addition and editing
#pragma {
void MainWindow::on_AddCharacter_clicked()
{
    if(!m_project->IsProjectOpen())
    {
        QueryMakeNewProject();
    }
    if(m_project->IsProjectOpen())
    {
        AddCharacterDialog addNewChar(this);
        int result = QDialog::DialogCode::Accepted;
        while(result == QDialog::DialogCode::Accepted)
        {
            result = addNewChar.exec();
            if(auto message = m_project->AddNewCharacter(addNewChar.getCharName().toStdString(),addNewChar.getBio().toStdString(), addNewChar.getImagePath().toStdString()); message != std::nullopt)
            {
                QString error = "The following error was return:\n";
                error += QString(message.value().c_str());
                error += "\nRetry?";
                auto decision = QMessageBox::information(this, "Error", error, QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No);
                if(decision == QMessageBox::StandardButton::Ok)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if(m_project->HasUnsavedChanges())
            {
                ui->actionSave->setEnabled(true);

                UpdateCurrentProject();
                break;
            }
        }
    }
}

void MainWindow::on_AddCharacterTake_clicked()
{

}
#pragma }


void MainWindow::on_AddInstallment_clicked()
{
    if(!m_project->IsProjectOpen())
    {
        QueryMakeNewProject();
    }
    if(m_project->IsProjectOpen())
    {
        //AddCharacterDialog addNewChar(this);
        //int result = addNewChar.exec();
        //if(result == QDialog::DialogCode::Accepted)
        //{
        //    m_project->AddNewCharacter(addNewChar.getCharName().toStdString(),addNewChar.getBio().toStdString(), addNewChar.getImagePath().toStdString());
        //}
    }
}
