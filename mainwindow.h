#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

#include "projectfile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    //Handy enum for tracking task completion state across functions
    enum CompletionState
    {
        None,
        Cancelled,
        Failure,
        Success,
        Retry,
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();              //New project

    void on_actionOpen_triggered();             //Open project

    void on_actionClose_triggered();            //Close project

    void on_actionExport_triggered();           //Export report

    void on_actionRecent_triggered();           //recent project list

    void on_actionUndo_triggered();             //undo last action in edit list

    void on_actionPreferences_triggered();      //open application preferences

    void on_actionRedo_triggered();             //redo an undone action

    void on_actionBuild_Dialogues_triggered();  //build all (missing) dialogues

    void on_actionMinimize_triggered();         //minimize the window

    void on_actionMaximize_triggered();         //maximize the window

    void on_actionRestore_triggered();          //restore the window

    void on_actionAbout_triggered();            //open about dialog

    void on_AddCharacter_clicked();             //add new character

    void on_AddCharacterTake_clicked();         //add new character take

    void on_AddInstallment_clicked();           //add new installment

    void on_actionSave_triggered();             //save the project

private:
    void UpdateCurrentProject();                    //Sets the window title and enables/disables controls
    void UpdateRecentList();                        //updates the recent project list
    bool MakeNewProject(QString targetDirectory);   //Creates a Characters & Installments directory in the target directory
    void QueryMakeNewProject();                     //Queries if the user wants to make a new project

    CompletionState OpenExistingProject(QString projectLocation);  //Opens an existing project
    void UpdateCharacterList();                     //Updates the available list of characters
    void UpdateInstallmentList();                   //Updates the available list of installments

private:
    Ui::MainWindow *ui;                     //QT generated ui object reference

    std::unique_ptr<ProjectFile> m_project; //the project object, if any

    std::unique_ptr<QStandardItemModel> m_characterList;
    std::unique_ptr<QStandardItemModel> m_installmentList;
};
#endif // MAINWINDOW_H
