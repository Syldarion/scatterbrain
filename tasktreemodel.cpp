#include "tasktreemodel.h"
#include "taskitem.h"

#include <QtWidgets>
#include <QJsonDocument>

TaskTreeModel::TaskTreeModel(const QStringList& headers, QObject* parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << "Root";
    rootData << "NONE";
    rootData << false;

    headerStrings = headers;

    rootItem = new TaskItem(rootData);
}

TaskTreeModel::~TaskTreeModel()
{
    delete rootItem;
}

int TaskTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

QVariant TaskTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TaskItem* item = getItem(index);

    if (role == Qt::CheckStateRole && index.column() == 2)
        return static_cast<int>(item->data(2).toBool() ? Qt::Checked : Qt::Unchecked);

    return item->data(index.column());
}

Qt::ItemFlags TaskTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | QAbstractItemModel::flags(index);

    if (index.column() == 2)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

TaskItem* TaskTreeModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        TaskItem* item = static_cast<TaskItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant TaskTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headerStrings[section];

    return QVariant();
}

QModelIndex TaskTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    TaskItem* parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    TaskItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

bool TaskTreeModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
    beginInsertColumns(parent, position, position + columns - 1);
    const bool success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool TaskTreeModel::insertRows(int position, int rows, const QModelIndex& parent)
{
    TaskItem* parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position + rows - 1);
    const bool success = parentItem->insertChildren(position,
                                                    rows,
                                                    rootItem->columnCount());
    endInsertRows();

    return success;
}

QModelIndex TaskTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();

    TaskItem* childItem = getItem(index);
    TaskItem* parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TaskTreeModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
    beginRemoveColumns(parent, position, position + columns - 1);
    const bool success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool TaskTreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
    TaskItem* parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int TaskTreeModel::rowCount(const QModelIndex& parent) const
{
    const TaskItem* parentItem = getItem(parent);

    return parentItem ? parentItem->childCount() : 0;
}

bool TaskTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TaskItem* item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return result;
}

bool TaskTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    headerStrings[section] = value.toString();
    emit headerDataChanged(orientation, section, section);

    return true;
}

void TaskTreeModel::read(const QJsonObject &json)
{
    if (json.contains("projectName"))
        projectName = json["projectName"].toString();
    if (json.contains("taskdata"))
        rootItem->read(json["taskdata"].toObject());
}

void TaskTreeModel::write(QJsonObject &json) const
{
    json["projectName"] = projectName;
    QJsonObject taskData = json["taskdata"].toObject();
    rootItem->write(taskData);
    json["taskdata"] = taskData;
}

bool TaskTreeModel::indexIsCompleted(const QModelIndex &index)
{
    TaskItem* item = getItem(index);
    return item->data(2).toBool();
}

int TaskTreeModel::completedTasks()
{
    return rootItem->completedDescendants();
}

int TaskTreeModel::totalTasks()
{
    return rootItem->totalDescendants();
}

QString TaskTreeModel::getProjectName()
{
    return projectName;
}
