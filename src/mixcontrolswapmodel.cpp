#include "mixcontrolswapmodel.h"
#include "pdebug.h"
#include "simul6.h"
#include "colorsgenerator.h"
#include "idgenerator.h"
#include "json.h"

#define ConstituentRole Qt::UserRole+1
#define SegmentsRole Qt::UserRole+2


MixControlSwapModel::MixControlSwapModel(QObject *parent)
    : MixControlModelAbstract(parent)
{
    insertColumns(0, LastCol);
    setHeaderData(Visible, Qt::Horizontal, tr("👁"));
    setHeaderData(Name, Qt::Horizontal, tr("Name"));
    setHeaderData(Color, Qt::Horizontal, tr(""));
    setHeaderData(NegCharge, Qt::Horizontal, tr("Neg"));
    setHeaderData(PosCharge, Qt::Horizontal, tr("Pos"));
    setHeaderData(Concentrations, Qt::Horizontal, tr("Concentrations"));
}


void MixControlSwapModel::setConstituent(const SegmentedConstituent& constituent, int row) {
    double concentration = (constituent.segments.size() == 0) ? 0  
                         : (constituent.segments.size() == 1) ? constituent.segments[0].concentration
                         : (constituent.segments.size() == 2) ? constituent.segments[1].concentration
                         :  constituent.segments[1].concentration;
    setData(index(row, 0), QVariant::fromValue(constituent), ConstituentRole);
    setData(index(row, Visible), constituent.visible());
    setData(index(row, Name), constituent.getName());
    setData(index(row, NegCharge), constituent.getNegCharge());
    setData(index(row, PosCharge), constituent.getPosCharge());
    setData(index(row, Concentrations), concentration);
    setData(index(row, Color), QBrush(constituent.color()), Qt::BackgroundRole);

}


void MixControlSwapModel::recalculate(const QList<int>& ratios) {
    // PDEBUG << ratios;
    for (int row=0; row<rowCount(); row++) {
        SegmentedConstituent x = constituent(row);
        x.setRatios(ratios);
        setConstituent(x, row);
        }
}


