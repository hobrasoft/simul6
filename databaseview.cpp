#include "databaseview.h"

DatabaseView::DatabaseView(QWidget *parent) : QTreeView(parent)
{
}

void DatabaseView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);
    int row = current.row();
    emit currentRowChanged(row);
}
