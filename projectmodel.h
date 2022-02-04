#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QAbstractListModel>

class Project;

class ProjectModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ProjectModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

    Project* getProject();
    void setProject(Project* project);
    QString getProjectName();
    void setProjectName(QString name);

    QString getTaskTitle(int row);
    QString getTaskDescription(int row);
    bool getTaskComplete(int row);
    void setTaskTitle(int row, QString title);
    void setTaskDescription(int row, QString description);
    void setTaskComplete(int row, bool complete);
private:
    Project* modelProject;
};

#endif // PROJECTMODEL_H
