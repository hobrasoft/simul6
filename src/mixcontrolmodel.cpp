#include "mixcontrolmodel.h"
#include "pdebug.h"
#include "simul6.h"
#include "colorsgenerator.h"
#include "idgenerator.h"
#include "json.h"

#define ConstituentRole Qt::UserRole+1
#define SegmentsRole Qt::UserRole+2


MixControlModel::MixControlModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertColumns(0, LastCol);
    setHeaderData(Visible, Qt::Horizontal, tr("👁"));
    setHeaderData(Name, Qt::Horizontal, tr("Name"));
    setHeaderData(Color, Qt::Horizontal, tr(""));
    setHeaderData(NegCount, Qt::Horizontal, tr("Neg"));
    setHeaderData(PosCount, Qt::Horizontal, tr("Pos"));
    setHeaderData(SegCount, Qt::Horizontal, tr("Segments"));
    setHeaderData(Concentrations, Qt::Horizontal, tr("Concentrations"));
    setHeaderData(Ratio, Qt::Horizontal, tr("Ratio"));
}

QModelIndex MixControlModel::add(const SegmentedConstituent& pConstituent) {
    insertRows(0, 1);
    SegmentedConstituent constituent = pConstituent;
    if (constituent.color() == QColor()) {
        constituent.setColor(ColorsGenerator::color());
        }
    if (constituent.getInternalId() == 0) {
        constituent.setInternalId(IdGenerator::nextId());
        }
    setConstituent(constituent, 0);
    return index(0, 0);
}

void MixControlModel::add(const QList<SegmentedConstituent>& constituents) {
    for (int i=0; i<constituents.size(); i++) {
        add(constituents[i]);
        }
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
        conc  << QString("%1").arg(constituent.segments[i].concentration, 0, 'f', 3);
    }

PDEBUG << constituent;
    setData(index(row, 0), QVariant::fromValue(constituent), ConstituentRole);
    setData(index(row, Visible), constituent.visible());
    setData(index(row, Name), constituent.getName());
    setData(index(row, NegCount), constituent.getNegCount());
    setData(index(row, PosCount), constituent.getPosCount());
    setData(index(row, SegCount), len.join("; "));
    setData(index(row, Concentrations), conc.join("; "));
    setData(index(row, Ratio), ratio.join("; "));
    setData(index(row, Color), QBrush(constituent.color()), Qt::BackgroundRole);

}


void MixControlModel::toggleVisible(const QModelIndex& idx) {
    if (idx.column() != Visible) { return; }
    QModelIndex vidx = index(idx.row(), Visible);
    int row = vidx.row();
    bool visible = data(vidx).toBool();
    bool negvisible = (visible) ? false : true;
    
    SegmentedConstituent c = constituent(row);
    c.setVisible(negvisible);
    setData(index(row, 0), QVariant::fromValue(c), ConstituentRole);

    setData(vidx, negvisible);

    emit visibilityChanged(c.getInternalId(), negvisible);

    // PDEBUG << c.getInternalId() << c.visible() << negvisible << constituent(row).visible();

}


SegmentedConstituent MixControlModel::constituent(int row) const {
    SegmentedConstituent sc = data(index(row, 0), ConstituentRole).value<SegmentedConstituent>();
    return sc;
}


QList<SegmentedConstituent> MixControlModel::constituents() const {
    QList<SegmentedConstituent> list;
    for (int i=0; i<rowCount(); i++) {
        list << constituent(i);
        }
    return list;
}


QVariantList MixControlModel::json() const {
    QVariantList list;
    for (int row=0; row<rowCount(); row++) {
        QVariantMap sample;
        sample["constituent"] = constituent(row).json();
        list << sample;
        }
    return list;
}


void MixControlModel::setJson(const QVariantList& list) {
    removeRows(0, rowCount());
    for (int i=0; i<list.size(); i++) {
        QVariantMap constituentMap = list[i].toMap()["constituent"].toMap();
        SegmentedConstituent constituent(constituentMap);
        add(constituent);
        }
}




