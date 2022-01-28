#ifndef TASKTREEMODEL_H
#define TASKTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TaskItem;

class TaskTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TaskTreeModel(const QStringList& headers, QObject* parent = nullptr);
    ~TaskTreeModel();

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

    bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    bool indexIsCompleted(const QModelIndex& index);
    int completedTasks();
    int totalTasks();
    int totalTopLevelTasks();
    void setProjectName(QString name);
    QString getProjectName();
private:
    QString projectName;
    TaskItem* getItem(const QModelIndex& index) const;
    TaskItem* rootItem;
    QStringList headerStrings;
};

#endif // TASKTREEMODEL_H
