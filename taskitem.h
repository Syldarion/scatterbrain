#ifndef TASKITEM_H
#define TASKITEM_H

#include <QVariant>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

class TaskItem
{
public:
    TaskItem();
    TaskItem(TaskItem& copy);
    ~TaskItem();

    QString getTitle();
    QString getDescription();
    bool getComplete();

    void setTitle(QString title);
    void setDescription(QString description);
    void setComplete(bool complete);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;
private:
    QString taskTitle;
    QString taskDescription;
    bool taskComplete;
};

#endif // TASKITEM_H
