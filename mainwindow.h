#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "projectmodelproxy.h"
#include "projectmodel.h"
#include "project.h"
#include "settingswindow.h"
#include "aboutwindow.h"
#include "contactwindow.h"

#include <QMainWindow>
#include <QContextMenuEvent>
#include <QSortFilterProxyModel>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    // Project Slots
    void saveProject();
    void saveProjectAs();
    void newProject();
    void openProject();
    void openRandomProject();
    void renameOpenProject();

    // Task Slots
    void addNewTask();
    void deleteCurrentTask();
    void completeCurrentTask();

    // Window Slots
    void openSettingsWindow();
    void openContactDialog();
    void openAboutDialog();

    // UI Slots
    void updateCurrentItemTitle();
    void updateCurrentItemDescription();
    void changeCompletedFilter(int state);
    void updateTaskView();
    void updateActions();

    // Other Slots
    void setModelDirty();
    void closeApplication();
private:
    void connectModelSignals();
    void connectActionSignals();
    void connectUiSignals();

    void discoverAvailableProjects();
    void openProjectAtLocation(QString path);
    void loadModel(ProjectModel* model);
    void toggleProjectActions(bool enabled);
    bool checkDirtyModel();
    void clearTaskView();

    bool hasFileOpen;
    QString currentOpenFilePath;
    SettingsWindow* settingsWindow;
    bool isModelDirty;
    QStringList availableProjectPaths;
    ProjectModelProxy* proxyModel;
};
#endif // MAINWINDOW_H
