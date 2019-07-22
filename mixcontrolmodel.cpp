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

void MixControlModel::add(const Constituent& constituent, const Segments& segments) {
    QStringList ratio;
    QStringList len;
    QStringList conc;
    for (int i=0; i<segments.segments.size(); i++) {
        ratio << QString("%1").arg(segments.segments[i].ratio, 0, 'f', 2);
        len   << QString("%1").arg(segments.segments[i].length, 0, 'f', 2);
        conc  << QString("%1").arg(segments.segments[i].concentration, 0, 'f', 2);
    }

    insertRows(0, 1);
    setData(index(0, Name), constituent.getName());
    setData(index(0, NegCount), constituent.getNegCount());
    setData(index(0, PosCount), constituent.getPosCount());
    setData(index(0, SegCount), len.join("; "));
    setData(index(0, Concentrations), conc.join("; "));
    setData(index(0, Ratio), ratio.join("; "));
}
