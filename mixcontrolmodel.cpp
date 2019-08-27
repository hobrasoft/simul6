#include "mixcontrolmodel.h"
#include "pdebug.h"
#include "json.h"

#define ConstituentRole Qt::UserRole+1
#define SegmentsRole Qt::UserRole+2

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

QModelIndex MixControlModel::add(const Constituent& constituent, const Segments& segments) {
    insertRows(0, 1);
    setConstituentAndSegments(constituent, segments, 0);
    return index(0, 0);
}

void MixControlModel::setConstituentAndSegments(const Constituent& constituent, const Segments& segments, int row) {
    QStringList ratio;
    // QStringList len;
    QStringList conc;
    for (int i=0; i<segments.segments.size(); i++) {
        ratio << QString("%1").arg(segments.segments[i].ratio, 0, 'f', 2);
        // len   << QString("%1").arg(segments.segments[i].length, 0, 'f', 2);
        conc  << QString("%1").arg(segments.segments[i].concentration, 0, 'f', 2);
    }

    setData(index(row, 0), QVariant::fromValue(constituent), ConstituentRole);
    setData(index(row, 0), QVariant::fromValue(segments), SegmentsRole);
    setData(index(row, Name), constituent.getName());
    setData(index(row, NegCount), constituent.getNegCount());
    setData(index(row, PosCount), constituent.getPosCount());
    // setData(index(row, SegCount), len.join("; "));
    setData(index(row, Concentrations), conc.join("; "));
    setData(index(row, Ratio), ratio.join("; "));

}


Constituent MixControlModel::constituent(int row) const {
    return data(index(row, 0), ConstituentRole).value<Constituent>();
}


Segments MixControlModel::segments(int row) const {
    return data(index(row, 0), SegmentsRole).value<Segments>();
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




