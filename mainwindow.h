#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "tasktreemodel.h"
#include "settingswindow.h"

#include <QMainWindow>
#include <QContextMenuEvent>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
public slots:
    void updateInterface();
private slots:
    void insertChild();
    bool insertColumn();
    void insertRow();
    bool removeColumn();
    void removeRow();
    void saveProject();
    void saveProjectAs();
    void newProject();
    void openProject();
    void openSettings();
    void closeApplication();
private:
    void loadModel(TaskTreeModel* model);
    bool hasFileOpen;
    QString currentOpenFilePath;
    SettingsWindow* settingsWindow;
};
#endif // MAINWINDOW_H
