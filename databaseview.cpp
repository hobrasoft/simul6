#include "databaseview.h"

DatabaseView::DatabaseView(QWidget *parent) : QTreeView(parent)
{
}

void DatabaseView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    int row = previous.row();
    emit currentRowChanged(row);
}
