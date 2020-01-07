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

QModelIndex MixControlModel::add(const Constituent& pConstituent, const Segments& segments) {
    insertRows(0, 1);
    Constituent constituent = pConstituent;
    if (constituent.color() == QColor()) {
        constituent.setColor(ColorsGenerator::color());
        }
    if (constituent.getInternalId() == 0) {
        constituent.setInternalId(IdGenerator::nextId());
        }
    setConstituentAndSegments(constituent, segments, 0);
    return index(0, 0);
}

void MixControlModel::add(const QList<Constituent>& constituents, const QList<Segments>& segments) {
    Q_ASSERT(constituents.size() == segments.size());
    for (int i=0; i<constituents.size(); i++) {
        add(constituents[i], segments[i]);
        }
}

void MixControlModel::setConstituentAndSegments(const Constituent& constituent, const Segments& segments, int row) {
    double caplen = Simul6::instance()->getCaplen();
    double ratioSum = 0;
    QStringList ratio;
    QStringList len;
    QStringList conc;
    for (int i=0; i<segments.segments.size(); i++) {
        double ratio = segments.segments[i].ratio;
        ratioSum += ratio;
    }
    for (int i=0; i<segments.segments.size(); i++) {
        double ratioLen = (ratioSum > 0) ? 1000 * segments.segments[i].ratio * caplen / ratioSum : 0;
        ratio << QString("%1").arg(segments.segments[i].ratio, 0, 'f', 2);
        len   << QString("%1").arg(ratioLen, 0, 'f', 2);
        conc  << QString("%1").arg(segments.segments[i].concentration, 0, 'f', 3);
    }

    setData(index(row, 0), QVariant::fromValue(constituent), ConstituentRole);
    setData(index(row, 0), QVariant::fromValue(segments), SegmentsRole);
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
    
    Constituent c = constituent(row);
    c.setVisible(negvisible);
    setData(index(row, 0), QVariant::fromValue(c), ConstituentRole);

    setData(vidx, negvisible);

    emit visibilityChanged(c.getInternalId(), negvisible);

    // PDEBUG << c.getInternalId() << c.visible() << negvisible << constituent(row).visible();

}


Constituent MixControlModel::constituent(int row) const {
    return data(index(row, 0), ConstituentRole).value<Constituent>();
}


Segments MixControlModel::segments(int row) const {
    return data(index(row, 0), SegmentsRole).value<Segments>();
}


QList<Constituent> MixControlModel::constituents() const {
    QList<Constituent> list;
    for (int i=0; i<rowCount(); i++) {
        list << constituent(i);
        }
    return list;
}


QList<Segments> MixControlModel::segments() const {
    QList<Segments> list;
    for (int i=0; i<rowCount(); i++) {
        list << segments(i);
        }
    return list;
}


QVariantList MixControlModel::json() const {
    QVariantList list;
    for (int row=0; row<rowCount(); row++) {
        QVariantMap sample;
        sample["constituent"] = constituent(row).json();
        sample["segments"] = segments(row).json();
        list << sample;
        }
    return list;
}


void MixControlModel::setJson(const QVariantList& list) {
    removeRows(0, rowCount());
    for (int i=0; i<list.size(); i++) {
        QVariantMap constituentMap = list[i].toMap()["constituent"].toMap();
        QVariantList segmentsMap = list[i].toMap()["segments"].toList();
        Constituent constituent(constituentMap);
        Segments segments(segmentsMap);
        add(constituent, segments);
        }
}




