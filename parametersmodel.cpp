#include "parametersmodel.h"

ParametersModel::ParametersModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertRows(0, LastRow);
    setHeaderData(pKa, Qt::Vertical, tr("pKa"));
    setHeaderData(U, Qt::Vertical, tr("U"));
    insertColumns(0, 6);
    setHeaderData(0, Qt::Horizontal, "-3");
    setHeaderData(1, Qt::Horizontal, "-2");
    setHeaderData(2, Qt::Horizontal, "-1");
    setHeaderData(3, Qt::Horizontal, "+1");
    setHeaderData(4, Qt::Horizontal, "+2");
    setHeaderData(5, Qt::Horizontal, "+3");
}
