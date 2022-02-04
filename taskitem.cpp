#include "taskitem.h"

TaskItem::TaskItem()
    : taskTitle("New Task"), taskDescription("No Description"), taskComplete(false)
{

}

TaskItem::TaskItem(TaskItem& copy)
    : taskTitle(copy.taskTitle), taskDescription(copy.taskDescription), taskComplete(copy.taskComplete)
{

}

TaskItem::~TaskItem()
{

}

QString TaskItem::getTitle()
{
    return taskTitle;
}

QString TaskItem::getDescription()
{
    return taskDescription;
}

bool TaskItem::getComplete()
{
    return taskComplete;
}

void TaskItem::setTitle(QString title)
{
    taskTitle = title;
}

void TaskItem::setDescription(QString description)
{
    taskDescription = description;
}

void TaskItem::setComplete(bool complete)
{
    taskComplete = complete;
}

void TaskItem::read(const QJsonObject &json)
{
    if (json.contains("title") && json["title"].isString())
        taskTitle = json["title"].toString();
    if (json.contains("description") && json["description"].isString())
        taskDescription = json["description"].toString();
    if (json.contains("done") && json["done"].isBool())
        taskComplete = json["done"].toBool();
}

void TaskItem::write(QJsonObject &json) const
{
    json["title"] = taskTitle;
    json["description"] = taskDescription;
    json["done"] = taskComplete;
}
