#include "tasktreemodelproxy.h"
#include "tasktreemodel.h"

TaskTreeModelProxy::TaskTreeModelProxy(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

bool TaskTreeModelProxy::getFilterCompleted()
{
    return filterCompleted;
}

void TaskTreeModelProxy::setFilterCompleted(bool enableFilter)
{
    filterCompleted = enableFilter;
    invalidateFilter();
}

bool TaskTreeModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    TaskTreeModel* model = static_cast<TaskTreeModel*>(sourceModel());
    QModelIndex doneIndex = model->index(sourceRow, TaskTreeModel::DONE_COL, sourceParent);
    bool taskCompleted = model->data(doneIndex, Qt::DisplayRole).toBool();

    if (filterCompleted && taskCompleted)
        return false;

    return true;
}
