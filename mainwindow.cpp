#include "mainwindow.h"
#include "texteditwithfocusoutevent.h"

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
#include <QThread>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    discoverAvailableProjects();

    hasFileOpen = false;
    currentOpenFilePath = "";
    isModelDirty = false;

    proxyModel = new ProjectModelProxy(this);
    projectTaskView->setModel(proxyModel);
    projectTaskView->addActions({actionAdd_Task, actionDelete_Task, actionComplete_Task});
    projectNameLabel->addAction(actionRename_Project);

    connectModelSignals();
    connectActionSignals();
    connectUiSignals();

    updateTaskView();
    updateActions();
}

MainWindow::~MainWindow()
{
    delete proxyModel;
}

void MainWindow::saveProject()
{
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
    proxyModel->getProject()->write(modelObject);

    modelFile.write(QJsonDocument(modelObject).toJson());
    modelFile.close();

    std::ostringstream saveStr;
    saveStr << "Saved project to " << currentOpenFilePath.toStdString();
    statusBar()->showMessage(QString::fromStdString(saveStr.str()));

    isModelDirty = false;
}

void MainWindow::saveProjectAs()
{
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
    proxyModel->getProject()->write(modelObject);

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
    Project* project = new Project();
    ProjectModel* model = new ProjectModel();
    model->setProject(project);

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

void MainWindow::renameOpenProject()
{
    if (projectTaskView->model() == nullptr)
    {
        return;
    }

    bool ok;
    QString newName = QInputDialog::getText(this,
                                            tr("Rename Project"),
                                            tr("Project Name:"),
                                            QLineEdit::Normal,
                                            proxyModel->getProjectName(),
                                            &ok);
    if (ok && !newName.isEmpty())
    {
        proxyModel->setProjectName(newName);
        projectNameLabel->setText(proxyModel->getProjectName());
    }
}

void MainWindow::addNewTask()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();

    if (!model->insertRow(index.row()+1, index.parent()))
        return;

    projectTaskView->selectionModel()->setCurrentIndex(model->index(index.row() + 1, 0, index.parent()), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    projectTaskView->edit(projectTaskView->selectionModel()->currentIndex());
}

void MainWindow::deleteCurrentTask()
{
    const QModelIndex index = projectTaskView->selectionModel()->currentIndex();
    QAbstractItemModel* model = projectTaskView->model();
    model->removeRow(index.row(), index.parent());
}

void MainWindow::completeCurrentTask()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    const bool hasCurrent = currentIndex.isValid();

    if (hasCurrent)
    {
        proxyModel->setTaskComplete(currentIndex.row(), true);
    }

    updateTaskView();
}

void MainWindow::openSettingsWindow()
{
    SettingsWindow().exec();
}

void MainWindow::openContactDialog()
{
    ContactWindow().exec();
}

void MainWindow::openAboutDialog()
{
    AboutWindow().exec();
}

void MainWindow::updateCurrentItemTitle()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    const bool hasCurrent = currentIndex.isValid();

    if (hasCurrent)
    {
        proxyModel->setTaskTitle(currentIndex.row(), taskTitleText->text());
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
    const bool hasCurrent = currentIndex.isValid();

    if (hasCurrent)
    {
        proxyModel->setTaskDescription(currentIndex.row(), taskDescriptionText->toPlainText());
    }
}

void MainWindow::changeCompletedFilter(int state)
{
    proxyModel->setFilterCompleted(!(state == Qt::CheckState::Checked));
}

void MainWindow::updateTaskView()
{
    const bool hasModel = projectTaskView->model() != nullptr;

    if (!hasModel)
    {
        taskTitleText->setEnabled(false);
        taskDescriptionText->setEnabled(false);

        return;
    }

    QModelIndex currentIndex = projectTaskView->selectionModel()->currentIndex();
    const bool hasCurrent = currentIndex.isValid();

    taskTitleText->setEnabled(hasCurrent);
    taskDescriptionText->setEnabled(hasCurrent);

    if (hasCurrent)
    {
        QString titleData = proxyModel->getTaskTitle(currentIndex.row());
        QString descData = proxyModel->getTaskDescription(currentIndex.row());
        bool doneData = proxyModel->getTaskComplete(currentIndex.row());

        taskTitleText->setText(titleData);
        taskDescriptionText->setText(descData);

        taskTitleText->setEnabled(!doneData);
        taskDescriptionText->setEnabled(!doneData);
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
    const bool hasCurrent = currentIndex.isValid();
    const bool taskIsComplete = hasCurrent && proxyModel->getTaskComplete(currentIndex.row());

    actionComplete_Task->setEnabled(!taskIsComplete);
}

void MainWindow::setModelDirty()
{
    isModelDirty = true;
}

void MainWindow::closeApplication()
{
    if(!checkDirtyModel())
        return;
    this->close();
}

void MainWindow::connectModelSignals()
{
    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::updateActions);
    connect(projectTaskView->model(), &QAbstractItemModel::dataChanged, this, &MainWindow::setModelDirty);

    connect(projectTaskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateTaskView);
    connect(projectTaskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);

    connect(proxyModel, &QSortFilterProxyModel::dataChanged, this, &MainWindow::updateTaskView);
}

void MainWindow::connectActionSignals()
{
    // Project Actions
    connect(actionSave_Project, &QAction::triggered, this, &MainWindow::saveProject);
    connect(actionSave_Project_As, &QAction::triggered, this, &MainWindow::saveProjectAs);
    connect(actionNew_Project, &QAction::triggered, this, &MainWindow::newProject);
    connect(actionOpen_Project, &QAction::triggered, this, &MainWindow::openProject);
    connect(actionOpen_Random_Project, &QAction::triggered, this, &MainWindow::openRandomProject);
    connect(actionRename_Project, &QAction::triggered, this, &MainWindow::renameOpenProject);

    // Task Actions
    connect(actionAdd_Task, &QAction::triggered, this, &MainWindow::addNewTask);
    connect(actionDelete_Task, &QAction::triggered, this, &MainWindow::deleteCurrentTask);
    connect(actionComplete_Task, &QAction::triggered, this, &MainWindow::completeCurrentTask);

    // Other Actions
    connect(actionSettings, &QAction::triggered, this, &MainWindow::openSettingsWindow);
    connect(actionQuit, &QAction::triggered, this, &MainWindow::closeApplication);
    connect(actionAbout, &QAction::triggered, this, &MainWindow::openAboutDialog);
    connect(actionContact, &QAction::triggered, this, &MainWindow::openContactDialog);
}

void MainWindow::connectUiSignals()
{
    connect(taskTitleText, &QLineEdit::editingFinished, this, &MainWindow::updateCurrentItemTitle);
    connect(taskDescriptionText, &TextEditWithFocusOutEvent::lostFocus, this, &MainWindow::updateCurrentItemDescription);
    connect(showCompletedTasksCheckbox, &QCheckBox::stateChanged, this, &MainWindow::changeCompletedFilter);
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

    Project* project = new Project();
    project->read(modelDoc.object());
    ProjectModel* projectModel = new ProjectModel(this);
    projectModel->setProject(project);

    loadModel(projectModel);

    hasFileOpen = true;
    currentOpenFilePath = path;

    std::ostringstream openStr;
    openStr << "Opened project from " << currentOpenFilePath.toStdString();
    statusBar()->showMessage(QString::fromStdString(openStr.str()));

    isModelDirty = false;
}

void MainWindow::loadModel(ProjectModel* model)
{
    proxyModel->setSourceModel(model);
    changeCompletedFilter(showCompletedTasksCheckbox->checkState());

    projectNameLabel->setText(model->getProjectName());
}

void MainWindow::toggleProjectActions(bool enabled)
{
    actionSave_Project_As->setEnabled(enabled);
    actionSave_Project->setEnabled(enabled);
    actionAdd_Task->setEnabled(enabled);
    actionDelete_Task->setEnabled(enabled);
    actionComplete_Task->setEnabled(enabled);
    actionRename_Project->setEnabled(enabled);
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
