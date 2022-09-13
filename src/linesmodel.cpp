/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "linesmodel.h"
#include "pdebug.h"

LinesModel::LinesModel(QObject *parent) : QStandardItemModel(parent) {
    insertColumns(0, LastCol);
    setHeaderData(Color,    Qt::Horizontal, tr("Color"));
    setHeaderData(Position, Qt::Horizontal, tr("Position"));
}

