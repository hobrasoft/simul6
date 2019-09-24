#include "myview.h"

MyView::MyView(QWidget *parent) : QTreeView(parent)
{
}

void MyView::currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);
    if (!current.isValid()) {
        emit currentRowChanged(-1);
    }
    int row = current.row();
    QItemSelectionModel sm;
    sm.select(current, QItemSelectionModel::Rows);
    setSelectionModel(&sm);
    emit currentRowChanged(row);
}
