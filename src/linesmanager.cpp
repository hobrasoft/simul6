/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "linesmanager.h"
#include "linesdelegate.h"
#include "simul6.h"
#include "ui_linesmanager.h"

LinesManager::LinesManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinesManager)
{
    ui->setupUi(this);
    ui->f_view->setModel(Simul6::instance()->linesModel());
    ui->f_view->setItemDelegate(new LinesDelegate(this));
    ui->f_remove->setEnabled(model()->rowCount() != 0);
    connect(ui->f_add, &QPushButton::clicked, this, &LinesManager::add);
    connect(ui->f_remove, &QPushButton::clicked, this, &LinesManager::remove);
}


LinesModel *LinesManager::model() const {
    return qobject_cast<LinesModel*>(ui->f_view->model());
}


void LinesManager::add() {
    model()->insertRows(0, 1);
    ui->f_remove->setEnabled(model()->rowCount() != 0);
    model()->setData(model()->index(0, model()->Color), QColor(0,0,0));
    model()->setData(model()->index(0, model()->Position), 1.0);
}


void LinesManager::remove() {
    QModelIndexList selectedRows = ui->f_view->selectionModel()->selectedRows();
    QMap<int, int> rowsToRemove;
    for (int i=0; i<selectedRows.size(); i++) {
        rowsToRemove[selectedRows[i].row()] = 0;
        }
    QMapIterator<int, int> iterator(rowsToRemove);
    iterator.toBack();
    while (iterator.hasPrevious()) {
        iterator.previous();
        int row = iterator.key();
        model()->removeRows(row, 1);
        }

    ui->f_remove->setEnabled(model()->rowCount() != 0);
}


