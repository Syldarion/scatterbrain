#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "tasktreemodel.h"

#include <QMainWindow>
#include <QContextMenuEvent>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
public slots:
    void updateInterface();
protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent* event) override;
#endif // QT_NO_CONTEXTMENU
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
private:
    void loadModel(TaskTreeModel* model);
    bool hasFileOpen;
    QString currentOpenFilePath;
};
#endif // MAINWINDOW_H
