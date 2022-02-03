#ifndef TASKTREEMODELPROXY_H
#define TASKTREEMODELPROXY_H

#include <QSortFilterProxyModel>

class TaskTreeModelProxy : public QSortFilterProxyModel
{
public:
    explicit TaskTreeModelProxy(QObject *parent = nullptr);

    bool getFilterCompleted();
    void setFilterCompleted(bool completed);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
private:
    bool filterCompleted;
};

#endif // TASKTREEMODELPROXY_H
