#include "project.h"
#include "taskitem.h"

Project::Project(QString name)
{
    projectName = name;
}

TaskItem* Project::getTask(int index)
{
    return tasks[index];
}

TaskItem Project::getTaskCopy(int index)
{
    return TaskItem(*tasks[index]);
}

int Project::taskCount()
{
    return tasks.count();
}

void Project::addTaskAt(int index, TaskItem *item)
{
    tasks.insert(index, item);
}

void Project::removeTaskAt(int index)
{
    delete tasks[index];
    tasks.removeAt(index);
}

QString Project::getProjectName()
{
    return projectName;
}

void Project::setProjectName(QString name)
{
    projectName = name;
}

void Project::read(const QJsonObject &json)
{
    if (json.contains("name") && json["name"].isString())
        projectName = json["name"].toString();

    if (json.contains("tasks") && json["tasks"].isArray())
    {
        QJsonArray taskArray = json["tasks"].toArray();

        for (int index = 0; index < tasks.count(); index++)
        {
            delete tasks[index];
        }

        tasks.clear();

        for (int index = 0; index < taskArray.count(); index++)
        {
            QJsonObject taskObject = taskArray[index].toObject();
            tasks.append(new TaskItem());
            tasks[index]->read(taskObject);
        }
    }
}

void Project::write(QJsonObject &json) const
{
    json["name"] = projectName;

    QJsonArray taskArray;

    for (int index = 0; index < tasks.count(); index++)
    {
        QJsonObject taskObject;
        tasks[index]->write(taskObject);
        taskArray.append(taskObject);
    }

    json["tasks"] = taskArray;
}
