#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

class TaskItem;

class Project
{
public:
    Project(QString name = "NEW PROJECT");

    TaskItem* getTask(int index);
    TaskItem getTaskCopy(int index);
    int taskCount();

    void addTaskAt(int index, TaskItem* item);
    void removeTaskAt(int index);

    QString getProjectName();
    void setProjectName(QString name);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;
private:
    QString projectName;
    QVector<TaskItem*> tasks;
};

#endif // PROJECT_H
