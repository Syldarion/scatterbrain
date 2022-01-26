#include "mainwindow.h"
#include "tasktreemodel.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <sstream>
#include <QString>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    connect(projectTaskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateInterface);
    connect(actionSave_Project, &QAction::triggered, this, &MainWindow::saveProject);
    connect(actionNew_Project, &QAction::triggered, this, &MainWindow::newProject);
    connect(actionOpen_Project, &QAction::triggered, this, &MainWindow::openProject);

    connect(actionAdd_Task, &QAction::triggered, this, &MainWindow::insertRow);
    connect(actionAdd_Child_Task, &QAction::triggered, this, &MainWindow::insertChild);
    connect(actionDelete_Task, &QAction::triggered, this, &MainWindow::removeRow);
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    menu.addAction(actionAdd_Task);
    menu.addAction(actionAdd_Child_Task);
    menu.addAction(actionDelete_Task);
    menu.addAction(actionComplete_Task);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::insertChild()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();

    if (model->columnCount(index) == 0)
    {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    projectTaskView->selectionModel()->setCurrentIndex(model->index(0, 0, index), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    updateInterface();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel* model = projectTaskView->model();
    int column = projectTaskView->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateInterface();

    return changed;
}

void MainWindow::insertRow()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    projectTaskView->selectionModel()->setCurrentIndex(model->index(index.row() + 1, 0, index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    updateInterface();
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel* model = projectTaskView->model();
    const int column = projectTaskView->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    const bool changed = model->removeColumn(column);
    if (changed)
        updateInterface();

    return changed;
}

void MainWindow::removeRow()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();
    if (model->removeRow(index.row(), index.parent()))
        updateInterface();
}

void MainWindow::updateInterface()
{
    const bool hasCurrent = projectTaskView->selectionModel()->currentIndex().isValid();
    TaskTreeModel* model = static_cast<TaskTreeModel*>(projectTaskView->model());
    const bool taskIsComplete = hasCurrent && !(model->indexIsCompleted(projectTaskView->selectionModel()->currentIndex()));

    actionComplete_Task->setEnabled(taskIsComplete);

    std::ostringstream taskStr;
    taskStr << model->completedTasks() << " / " << model->totalTasks() << " tasks completed.";
    statusBar()->showMessage(QString::fromStdString(taskStr.str()));
}

void MainWindow::saveProject()
{
    TaskTreeModel* model = static_cast<TaskTreeModel*>(projectTaskView->model());

    QFile modelFile("C:/Users/parag/Desktop/testdata.json");

    if (!modelFile.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open file.");
    }

    QJsonObject modelObject;
    model->write(modelObject);

    modelFile.write(QJsonDocument(modelObject).toJson());
    modelFile.close();
}

void MainWindow::newProject()
{
    const QStringList headers({tr("Title"), tr("Description"), tr("Done")});
    TaskTreeModel* model = new TaskTreeModel(headers);

    projectTaskView->setModel(model);

    // TODO: Do I need to remove old connects?
    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::updateInterface);
}

void MainWindow::openProject()
{
    QFile modelFile("C:/Users/parag/Desktop/testdata.json");

    if (!modelFile.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open file.");
    }

    QByteArray modelData = modelFile.readAll();
    modelFile.close();
    QJsonDocument modelDoc = QJsonDocument::fromJson(modelData);

    const QStringList headers({tr("Title"), tr("Description"), tr("Done")});
    TaskTreeModel* model = new TaskTreeModel(headers);
    model->read(modelDoc.object());

    projectTaskView->setModel(model);
    for (int column = 0; column < model->columnCount(); column++)
        projectTaskView->resizeColumnToContents(column);

    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::updateInterface);
}
