#include "parametersmodel.h"

ParametersModel::ParametersModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertRows(0, LastRow);
    setHeaderData(pKa, Qt::Vertical, tr("pKa"));
    setHeaderData(U, Qt::Vertical, tr("U"));
    insertColumns(0, 6);
    setHeaderData(N3, Qt::Horizontal, "-3");
    setHeaderData(N2, Qt::Horizontal, "-2");
    setHeaderData(N1, Qt::Horizontal, "-1");
    setHeaderData(P1, Qt::Horizontal, "+1");
    setHeaderData(P2, Qt::Horizontal, "+2");
    setHeaderData(P3, Qt::Horizontal, "+3");
}
