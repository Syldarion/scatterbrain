#ifndef PROJECTMODELPROXY_H
#define PROJECTMODELPROXY_H

#include <QSortFilterProxyModel>

class ProjectModel;
class Project;

class ProjectModelProxy : public QSortFilterProxyModel
{
public:
    explicit ProjectModelProxy(QObject *parent = nullptr);

    bool getFilterCompleted();
    void setFilterCompleted(bool completed);

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
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
private:
    ProjectModel* sourceAsProject() const;
    int mapRowToSource(int row);

    bool filterCompleted;
};

#endif // PROJECTMODELPROXY_H
