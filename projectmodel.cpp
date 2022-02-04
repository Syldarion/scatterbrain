#include "projectmodel.h"
#include "taskitem.h"
#include "project.h"

ProjectModel::ProjectModel(QObject *parent)
    : QAbstractListModel{parent}
{
    modelProject = nullptr;
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | QAbstractItemModel::flags(index);

    return flags;
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    return modelProject->getTaskCopy(index.row()).getTitle();
}

QVariant ProjectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    return modelProject->taskCount();
}

bool ProjectModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    modelProject->getTask(index.row())->setTitle(value.toString());
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return true;
}

bool ProjectModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    beginInsertRows(parent, position, position + rows - 1);

    for (int r = 0; r < rows; r++)
    {
        modelProject->addTaskAt(position, new TaskItem());
    }

    endInsertRows();

    return true;
}

bool ProjectModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    beginRemoveRows(parent, position, position + rows - 1);

    for (int r = 0; r < rows; r++)
    {
        modelProject->removeTaskAt(position);
    }

    endRemoveRows();

    return true;
}

Project* ProjectModel::getProject()
{
    return modelProject;
}

void ProjectModel::setProject(Project *project)
{
    if (modelProject != nullptr)
    {
        delete modelProject;
    }

    modelProject = project;
}

QString ProjectModel::getProjectName()
{
    return modelProject->getProjectName();
}

void ProjectModel::setProjectName(QString name)
{
    modelProject->setProjectName(name);
}

QString ProjectModel::getTaskTitle(int row)
{
    return modelProject->getTask(row)->getTitle();
}

QString ProjectModel::getTaskDescription(int row)
{
    return modelProject->getTask(row)->getDescription();
}

bool ProjectModel::getTaskComplete(int row)
{
    return modelProject->getTask(row)->getComplete();
}

void ProjectModel::setTaskTitle(int row, QString title)
{
    modelProject->getTask(row)->setTitle(title);
}

void ProjectModel::setTaskDescription(int row, QString description)
{
    modelProject->getTask(row)->setDescription(description);
}

void ProjectModel::setTaskComplete(int row, bool complete)
{
    modelProject->getTask(row)->setComplete(complete);
}
