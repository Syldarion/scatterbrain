#include "taskitem.h"

QVector<QVariant> TaskItem::defaultValues({"New Task", "No Description", false});

TaskItem::TaskItem(const QVector<QVariant>& data, TaskItem* parent)
    : itemData(data), parentItem(parent)
{

}

TaskItem::~TaskItem()
{
    qDeleteAll(childItems);
}

TaskItem* TaskItem::child(int number)
{
    if (number < 0 || number >= childItems.size())
        return nullptr;

    return childItems.at(number);
}

int TaskItem::childCount() const
{
    return childItems.count();
}

int TaskItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TaskItem*>(this));

    return 0;
}

int TaskItem::columnCount() const
{
    return 1;
}

QVariant TaskItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();

    return itemData.at(column);
}

bool TaskItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; row++)
    {
        TaskItem* item = new TaskItem(defaultValues, this);
        childItems.insert(position, item);
    }

    return true;
}

bool TaskItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; column++)
        itemData.insert(position, QVariant());

    for (TaskItem* child : qAsConst(childItems))
        child->insertColumns(position, columns);

    return true;
}

TaskItem* TaskItem::parent()
{
    return parentItem;
}

bool TaskItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; row++)
        delete childItems.takeAt(position);

    return true;
}

bool TaskItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; column++)
        itemData.remove(position);

    for (TaskItem* child : qAsConst(childItems))
        child->removeColumns(position, columns);

    return true;
}

bool TaskItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

void TaskItem::read(const QJsonObject &json)
{
    if (json.contains("title") && json["title"].isString())
        setData(0, json["title"].toVariant());
    if (json.contains("description") && json["description"].isString())
        setData(1, json["description"].toVariant());
    if (json.contains("done") && json["done"].isBool())
        setData(2, json["done"].toVariant());

    if (json.contains("children") && json["children"].isArray())
    {
        QJsonArray childArray = json["children"].toArray();
        removeChildren(0, childCount());
        insertChildren(0, childArray.size(), 3);
        for (int index = 0; index < childArray.size(); index++)
        {
            QJsonObject childObject = childArray[index].toObject();
            childItems[index]->read(childObject);
        }
    }
}

void TaskItem::write(QJsonObject &json) const
{
    json["title"] = QJsonValue::fromVariant(data(0));
    json["description"] = QJsonValue::fromVariant(data(1));
    json["done"] = QJsonValue::fromVariant(data(2));

    QJsonArray childArray;

    for (int index = 0; index < childItems.count(); index++)
    {
        QJsonObject childObject;
        childItems[index]->write(childObject);
        childArray.append(childObject);
    }

    json["children"] = childArray;
}

int TaskItem::completedDescendants()
{
    int completed = 0;

    for (int i = 0; i < childCount(); i++)
    {
        if (childItems[i]->data(2).toBool())
        {
            completed++;
        }

        completed += childItems[i]->completedDescendants();
    }

    return completed;
}

int TaskItem::totalDescendants()
{
    int total = 0;

    for (int i = 0; i < childCount(); i++)
    {
        total++;
        total += childItems[i]->totalDescendants();
    }

    return total;
}
