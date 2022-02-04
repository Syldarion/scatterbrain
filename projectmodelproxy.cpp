#include "projectmodelproxy.h"
#include "projectmodel.h"

ProjectModelProxy::ProjectModelProxy(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

bool ProjectModelProxy::getFilterCompleted()
{
    return filterCompleted;
}

void ProjectModelProxy::setFilterCompleted(bool enableFilter)
{
    filterCompleted = enableFilter;
    invalidateFilter();
}

Project* ProjectModelProxy::getProject()
{
    return static_cast<ProjectModel*>(sourceModel())->getProject();
}

void ProjectModelProxy::setProject(Project *project)
{
    static_cast<ProjectModel*>(sourceModel())->setProject(project);
}

QString ProjectModelProxy::getProjectName()
{
    return static_cast<ProjectModel*>(sourceModel())->getProjectName();
}

void ProjectModelProxy::setProjectName(QString name)
{
    static_cast<ProjectModel*>(sourceModel())->setProjectName(name);
}

QString ProjectModelProxy::getTaskTitle(int row)
{
    return sourceAsProject()->getTaskTitle(mapRowToSource(row));
}

QString ProjectModelProxy::getTaskDescription(int row)
{
    return sourceAsProject()->getTaskDescription(mapRowToSource(row));
}

bool ProjectModelProxy::getTaskComplete(int row)
{
    return sourceAsProject()->getTaskComplete(mapRowToSource(row));
}

void ProjectModelProxy::setTaskTitle(int row, QString title)
{
    sourceAsProject()->setTaskTitle(mapRowToSource(row), title);
}

void ProjectModelProxy::setTaskDescription(int row, QString description)
{
    sourceAsProject()->setTaskDescription(mapRowToSource(row), description);
}

void ProjectModelProxy::setTaskComplete(int row, bool complete)
{
    sourceAsProject()->setTaskComplete(mapRowToSource(row), complete);
}

bool ProjectModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (filterCompleted && sourceAsProject()->getTaskComplete(sourceRow))
        return false;

    return true;
}

ProjectModel* ProjectModelProxy::sourceAsProject() const
{
    return static_cast<ProjectModel*>(sourceModel());
}

int ProjectModelProxy::mapRowToSource(int row)
{
    return mapToSource(index(row, 0)).row();
}
