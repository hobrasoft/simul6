#include "mixcontrolmodelabstract.h"
#include "pdebug.h"
#include "simul6.h"
#include "colorsgenerator.h"
#include "idgenerator.h"
#include "json.h"

#define ConstituentRole Qt::UserRole+1
#define SegmentsRole Qt::UserRole+2

#define Visible 0

MixControlModelAbstract::MixControlModelAbstract(QObject *parent)
    : QStandardItemModel(parent)
{
}

QModelIndex MixControlModelAbstract::add(const SegmentedConstituent& pConstituent) {
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


void MixControlModelAbstract::add(const QList<SegmentedConstituent>& constituents) {
    for (int i=0; i<constituents.size(); i++) {
        add(constituents[i]);
        }
}


void MixControlModelAbstract::hide(int id) {
    for (int row=0; row<rowCount(); row++) {
        SegmentedConstituent c = constituent(row);
        if (c.getInternalId() == id) {
            c.setVisible(false);
            setData(index(row,0), QVariant::fromValue(c), ConstituentRole);
            setData(index(row,Visible), false);
            emit visibilityChanged(id, false);
            return;
            }
        }
}


void MixControlModelAbstract::disableConstituent(int id) {
    for (int row=0; row<rowCount(); row++) {
        SegmentedConstituent c = constituent(row);
        if (c.getInternalId() == id) {
            for (int col=0; col<columnCount(); col++) {
                item(row,col)->setEnabled(false);
                }
            return;
            }
        }
}


void MixControlModelAbstract::enableConstituents() {
    for (int row=0; row<rowCount(); row++) {
        for (int col=0; col<columnCount(); col++) {
            item(row,col)->setEnabled(true);
            }
        }
}


void MixControlModelAbstract::toggleVisible(const QModelIndex& idx) {
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


SegmentedConstituent MixControlModelAbstract::constituent(int row) const {
    SegmentedConstituent sc = data(index(row, 0), ConstituentRole).value<SegmentedConstituent>();
    return sc;
}


QList<SegmentedConstituent> MixControlModelAbstract::constituents() const {
    QList<SegmentedConstituent> list;
    for (int i=0; i<rowCount(); i++) {
        list << constituent(i);
        }
    return list;
}


QVariantList MixControlModelAbstract::json() const {
    QVariantList list;
    for (int row=0; row<rowCount(); row++) {
        QVariantMap sample;
        sample["constituent"] = constituent(row).json();
        list << sample;
        }
    return list;
}


void MixControlModelAbstract::setJson(const QVariantList& list) {
    removeRows(0, rowCount());
    for (int i=0; i<list.size(); i++) {
        QVariantMap constituentMap = list[i].toMap()["constituent"].toMap();
        SegmentedConstituent constituent(constituentMap);
        add(constituent);
        }
}




