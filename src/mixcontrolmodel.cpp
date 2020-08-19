#include "mixcontrolmodel.h"
#include "pdebug.h"
#include "simul6.h"
#include "colorsgenerator.h"
#include "idgenerator.h"
#include "json.h"

#define ConstituentRole Qt::UserRole+1
#define SegmentsRole Qt::UserRole+2


MixControlModel::MixControlModel(QObject *parent)
    : MixControlModelAbstract (parent)
{
    insertColumns(0, LastCol);
    setHeaderData(Visible, Qt::Horizontal, tr("👁"));
    setHeaderData(Name, Qt::Horizontal, tr("Name"));
    setHeaderData(Color, Qt::Horizontal, tr(""));
    setHeaderData(NegCharge, Qt::Horizontal, tr("Neg"));
    setHeaderData(PosCharge, Qt::Horizontal, tr("Pos"));
    setHeaderData(SegCount, Qt::Horizontal, tr("Segments"));
    setHeaderData(Concentrations, Qt::Horizontal, tr("Concentrations"));
    setHeaderData(Ratio, Qt::Horizontal, tr("Ratio"));
}


void MixControlModel::setConstituent(const SegmentedConstituent& constituent, int row) {
    double caplen = Simul6::instance()->getCaplen();
    double ratioSum = 0;
    QStringList ratio;
    QStringList len;
    QStringList conc;
    for (int i=0; i<constituent.segments.size(); i++) {
        double ratio = constituent.segments[i].ratio;
        ratioSum += ratio;
    }
    for (int i=0; i<constituent.segments.size(); i++) {
        double ratioLen = (ratioSum > 0) ? 1000 * constituent.segments[i].ratio * caplen / ratioSum : 0;
        ratio << QString("%1").arg(constituent.segments[i].ratio, 0, 'f', 2);
        len   << QString("%1").arg(ratioLen, 0, 'f', 2);
        conc  << QString("%1").arg(constituent.segments[i].concentration, 0, 'g', 8);
    }

    setData(index(row, 0), QVariant::fromValue(constituent), ConstituentRole);
    setData(index(row, Visible), constituent.visible());
    setData(index(row, Name), constituent.getName());
    setData(index(row, NegCharge), constituent.getNegCharge());
    setData(index(row, PosCharge), constituent.getPosCharge());
    setData(index(row, SegCount), len.join("; "));
    setData(index(row, Concentrations), conc.join("; "));
    setData(index(row, Ratio), ratio.join("; "));
    setData(index(row, Color), QBrush(constituent.color()), Qt::BackgroundRole);

}


