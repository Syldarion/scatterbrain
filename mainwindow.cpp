#include "mainwindow.h"
#include "settingswindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <sstream>
#include <QString>
#include <QFileDialog>
#include <QStandardPaths>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    discoverAvailableProjects();

    hasFileOpen = false;
    currentOpenFilePath = "";
    isModelDirty = false;

    proxyModel = new TaskTreeModelProxy(this);

    //UI stuff
    projectTaskView->addActions({actionAdd_Task, actionAdd_Child_Task, actionDelete_Task, actionComplete_Task});
    projectNameLabel->addAction(actionRename_Project);

    connect(actionSave_Project, &QAction::triggered, this, &MainWindow::saveProject);
    connect(actionSave_Project_As, &QAction::triggered, this, &MainWindow::saveProjectAs);
    connect(actionNew_Project, &QAction::triggered, this, &MainWindow::newProject);
    connect(actionOpen_Project, &QAction::triggered, this, &MainWindow::openProject);
    connect(actionOpen_Random_Project, &QAction::triggered, this, &MainWindow::openRandomProject);

    connect(actionAdd_Task, &QAction::triggered, this, &MainWindow::insertRow);
    connect(actionAdd_Child_Task, &QAction::triggered, this, &MainWindow::insertChild);
    connect(actionDelete_Task, &QAction::triggered, this, &MainWindow::removeRow);

    connect(actionRename_Project, &QAction::triggered, this, &MainWindow::renameOpenProject);

    connect(actionSettings, &QAction::triggered, this, &MainWindow::openSettings);
    connect(actionQuit, &QAction::triggered, this, &MainWindow::closeApplication);

    connect(completeTaskButton, &QPushButton::clicked, this, &MainWindow::completeCurrentItem);
    connect(applyTaskButton, &QPushButton::clicked, this, &MainWindow::applyTaskChanges);

    connect(showCompletedTasksCheckbox, &QCheckBox::stateChanged, this, &MainWindow::changeCompletedFilter);

    updateTaskView();
    updateActions();
}

MainWindow::~MainWindow()
{
    delete proxyModel;
}

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
    projectTaskView->edit(projectTaskView->selectionModel()->currentIndex());
}

void MainWindow::insertRow()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    projectTaskView->selectionModel()->setCurrentIndex(model->index(index.row() + 1, 0, index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    projectTaskView->edit(projectTaskView->selectionModel()->currentIndex());
}

void MainWindow::removeRow()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();
    model->removeRow(index.row(), index.parent());
}

void MainWindow::updateTaskView()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        taskTitleText->setEnabled(false);
        taskDescriptionText->setEnabled(false);
        completeTaskButton->setEnabled(false);
        deleteTaskButton->setEnabled(false);

        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
    const bool hasCurrent = currentIndex.isValid();

    taskTitleText->setEnabled(hasCurrent);
    taskDescriptionText->setEnabled(hasCurrent);
    completeTaskButton->setEnabled(hasCurrent);
    deleteTaskButton->setEnabled(hasCurrent);

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    if (hasCurrent)
    {
        bool taskDone = model->data(model->index(sourceIndex.row(), TaskTreeModel::DONE_COL), Qt::DisplayRole).toBool();

        taskTitleText->setText(model->data(model->index(sourceIndex.row(), TaskTreeModel::TITLE_COL), Qt::DisplayRole).toString());

        QModelIndex descIndex = model->index(sourceIndex.row(), TaskTreeModel::DESC_COL);
        QVariant descData = model->data(descIndex, Qt::DisplayRole);

        taskDescriptionText->setText(descData.toString());

        taskTitleText->setEnabled(!taskDone);
        taskDescriptionText->setEnabled(!taskDone);
        completeTaskButton->setEnabled(!taskDone);
    }
}

void MainWindow::updateActions()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    toggleProjectActions(hasModel);

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
    const bool hasCurrent = currentIndex.isValid();

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());
    const bool taskIsComplete = hasCurrent && !(model->indexIsCompleted(sourceIndex));

    actionComplete_Task->setEnabled(taskIsComplete);
}

void MainWindow::applyTaskChanges()
{
    updateCurrentItemTitle();
    updateCurrentItemDescription();
}

void MainWindow::updateCurrentItemTitle()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
    const bool hasCurrent = currentIndex.isValid();

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    if (hasCurrent)
    {
        model->setData(model->index(sourceIndex.row(), TaskTreeModel::TITLE_COL), taskTitleText->text());
    }
}

void MainWindow::updateCurrentItemDescription()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
    const bool hasCurrent = currentIndex.isValid();

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    if (hasCurrent)
    {
        model->setData(model->index(sourceIndex.row(), TaskTreeModel::DESC_COL), taskDescriptionText->toPlainText());
    }
}

void MainWindow::completeCurrentItem()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    QModelIndex sourceIndex = proxyModel->mapToSource(currentIndex);
    const bool hasCurrent = currentIndex.isValid();

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    if (hasCurrent)
    {
        model->setData(model->index(sourceIndex.row(), TaskTreeModel::DONE_COL), true);
    }

    updateTaskView();
}

void MainWindow::saveProject()
{
    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    if (!hasFileOpen)
    {
        saveProjectAs();
        return;
    }

    QFile modelFile(currentOpenFilePath);

    if (!modelFile.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open file.");
        return;
    }

    QJsonObject modelObject;
    model->write(modelObject);

    modelFile.write(QJsonDocument(modelObject).toJson());
    modelFile.close();

    std::ostringstream saveStr;
    saveStr << "Saved project to " << currentOpenFilePath.toStdString();
    statusBar()->showMessage(QString::fromStdString(saveStr.str()));

    isModelDirty = false;
}

void MainWindow::saveProjectAs()
{
    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());

    QString docPath = UserSettings::getInstance()->getProjectDirectory();
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Project As"),
                                                    hasFileOpen ? currentOpenFilePath : docPath,
                                                    tr("JSON files (*.json)"));

    QFile modelFile(fileName);

    if (!modelFile.open(QIODevice::WriteOnly))
    {
        qWarning("Could not open file.");
        return;
    }

    QJsonObject modelObject;
    model->write(modelObject);

    modelFile.write(QJsonDocument(modelObject).toJson());
    modelFile.close();

    hasFileOpen = true;
    currentOpenFilePath = fileName;

    std::ostringstream saveStr;
    saveStr << "Saved project to " << currentOpenFilePath.toStdString();
    statusBar()->showMessage(QString::fromStdString(saveStr.str()));

    isModelDirty = false;
}

void MainWindow::newProject()
{
    const QStringList headers({tr("Title"), tr("Description"), tr("Done")});
    TaskTreeModel* model = new TaskTreeModel(headers);

    bool ok;
    QString newName = QInputDialog::getText(this,
                                            tr("New Project"),
                                            tr("Project Name:"),
                                            QLineEdit::Normal,
                                            "New Project",
                                            &ok);

    if (!ok)
    {
        delete model;
        return;
    }

    model->setProjectName(newName);

    loadModel(model);

    hasFileOpen = false;
    currentOpenFilePath = "";
}

void MainWindow::openProject()
{
    QString docPath = UserSettings::getInstance()->getProjectDirectory();
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Project"),
                                                    hasFileOpen ? currentOpenFilePath : docPath,
                                                    tr("JSON files (*.json)"));

    openProjectAtLocation(fileName);
}

void MainWindow::openSettings()
{
    settingsWindow = new SettingsWindow();
    settingsWindow->show();
}

void MainWindow::closeApplication()
{
    if(!checkDirtyModel())
        return;
    this->close();
}

void MainWindow::renameOpenProject()
{
    if (projectTaskView->model() == nullptr)
    {
        return;
    }

    TaskTreeModel* model = static_cast<TaskTreeModel*>(proxyModel->sourceModel());
    bool ok;
    QString newName = QInputDialog::getText(this,
                                            tr("Rename Project"),
                                            tr("Project Name:"),
                                            QLineEdit::Normal,
                                            model->getProjectName(),
                                            &ok);
    if (ok && !newName.isEmpty())
    {
        model->setProjectName(newName);
        projectNameLabel->setText(model->getProjectName());
    }
}

void MainWindow::loadModel(TaskTreeModel* model)
{
    proxyModel->setSourceModel(model);
    projectTaskView->setModel(proxyModel);
    changeCompletedFilter(showCompletedTasksCheckbox->checkState());

    for (int column = 0; column < model->columnCount(); column++)
        projectTaskView->resizeColumnToContents(column);

    projectNameLabel->setText(model->getProjectName());

    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::updateActions);
    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::setModelDirty);

    connect(projectTaskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateTaskView);
    connect(projectTaskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);
}

void MainWindow::toggleProjectActions(bool enabled)
{
    actionSave_Project_As->setEnabled(enabled);
    actionSave_Project->setEnabled(enabled);
    actionAdd_Task->setEnabled(enabled);
    actionAdd_Child_Task->setEnabled(enabled);
    actionDelete_Task->setEnabled(enabled);
    actionComplete_Task->setEnabled(enabled);
    actionRename_Project->setEnabled(enabled);
}

void MainWindow::setModelDirty()
{
    isModelDirty = true;
}

void MainWindow::discoverAvailableProjects()
{
    QDir docDir(UserSettings::getInstance()->getProjectDirectory());
    QFileInfoList projectFileInfo = docDir.entryInfoList({tr("*.json")}, QDir::Files | QDir::Readable);
    foreach (const QFileInfo &info, projectFileInfo)
        availableProjectPaths << info.absoluteFilePath();
    QString statusMsg;
    statusMsg = QString("Found ") + QString::number(availableProjectPaths.count()) + QString(" projects in ") + docDir.absolutePath();
    statusBar()->showMessage(statusMsg);
}

void MainWindow::openRandomProject()
{
    int index;
    QString nextPath;

    do
    {
        index = QRandomGenerator::global()->bounded(availableProjectPaths.count());
        nextPath = availableProjectPaths[index];
    } while(nextPath == currentOpenFilePath);

    openProjectAtLocation(nextPath);
}

void MainWindow::openProjectAtLocation(QString path)
{
    if (!checkDirtyModel())
        return;

    QFile modelFile(path);

    if (!modelFile.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open file.");
        return;
    }

    QByteArray modelData = modelFile.readAll();
    modelFile.close();
    QJsonDocument modelDoc = QJsonDocument::fromJson(modelData);

    const QStringList headers({tr("Title"), tr("Description"), tr("Done")});
    TaskTreeModel* model = new TaskTreeModel(headers);
    model->read(modelDoc.object());

    loadModel(model);

    hasFileOpen = true;
    currentOpenFilePath = path;

    std::ostringstream openStr;
    openStr << "Opened project from " << currentOpenFilePath.toStdString();
    statusBar()->showMessage(QString::fromStdString(openStr.str()));

    isModelDirty = false;
}

bool MainWindow::checkDirtyModel()
{
    if (isModelDirty)
    {
        QMessageBox saveBox;
        saveBox.setText("The open project has been modified.");
        saveBox.setInformativeText("Do you want to save your changes?");
        saveBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        saveBox.setDefaultButton(QMessageBox::Save);
        int ret = saveBox.exec();

        switch(ret)
        {
        case QMessageBox::Save:
            saveProject();
            break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return false;
        default:
            break;
        }
    }

    return true;
}

void MainWindow::changeCompletedFilter(int state)
{
    proxyModel->setFilterCompleted(!(state == Qt::CheckState::Checked));
}

