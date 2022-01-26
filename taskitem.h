#ifndef TASKITEM_H
#define TASKITEM_H

#include <QVariant>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

class TaskItem
{
public:
    explicit TaskItem(const QVector<QVariant> &data, TaskItem* parent = nullptr);
    ~TaskItem();

    TaskItem* child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    TaskItem* parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant& value);

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;
    int completedDescendants();
    int totalDescendants();
private:
    static QVector<QVariant> defaultValues;
    QVector<TaskItem*> childItems;
    QVector<QVariant> itemData;
    TaskItem* parentItem;
};

#endif // TASKITEM_H
