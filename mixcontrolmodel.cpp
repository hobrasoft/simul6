#include "mixcontrolmodel.h"

MixControlModel::MixControlModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertColumns(0, LastCol);
    setHeaderData(Name, Qt::Horizontal, tr("Name"));
    setHeaderData(Color, Qt::Horizontal, tr(""));
    setHeaderData(NegCount, Qt::Horizontal, tr("Neg"));
    setHeaderData(PosCount, Qt::Horizontal, tr("Pos"));
    setHeaderData(SegCount, Qt::Horizontal, tr("Segments"));
    setHeaderData(Concentrations, Qt::Horizontal, tr("Concentrations"));
    setHeaderData(Ratio, Qt::Horizontal, tr("Ratio"));
}
