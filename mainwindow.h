#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "projectmodelproxy.h"
#include "projectmodel.h"
#include "project.h"
#include "settingswindow.h"

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
    void updateTaskView();
    void updateActions();

    void insertRow();
    void removeRow();

    void saveProject();
    void saveProjectAs();
    void newProject();
    void openProject();
    void openSettings();
    void closeApplication();
    void renameOpenProject();
    void discoverAvailableProjects();
    void openRandomProject();
    void openProjectAtLocation(QString path);
    bool checkDirtyModel();
    void updateCurrentItemTitle();
    void updateCurrentItemDescription();
    void completeCurrentItem();
    void applyTaskChanges();
    void changeCompletedFilter(int state);
private:
    void loadModel(ProjectModel* model);
    void toggleProjectActions(bool enabled);
    void setModelDirty();
    bool hasFileOpen;
    QString currentOpenFilePath;
    SettingsWindow* settingsWindow;
    bool isModelDirty;
    QStringList availableProjectPaths;
    ProjectModelProxy* proxyModel;
};
#endif // MAINWINDOW_H
