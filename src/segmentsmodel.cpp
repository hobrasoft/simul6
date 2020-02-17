#include "segmentsmodel.h"
#include "simul6.h"
#include "pdebug.h"

SegmentsModel::SegmentsModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertRows(0, LastRow);
    setHeaderData(Ratio, Qt::Vertical, tr("Ratio"));
    setHeaderData(Concentration, Qt::Vertical, tr("c [mM]"));
    setHeaderData(U3n, Qt::Vertical, tr("u-3"));
    setHeaderData(U2n, Qt::Vertical, tr("u-2"));
    setHeaderData(U1n, Qt::Vertical, tr("u-1"));
    setHeaderData(U1p, Qt::Vertical, tr("u+1"));
    setHeaderData(U2p, Qt::Vertical, tr("u+2"));
    setHeaderData(U3p, Qt::Vertical, tr("u+3"));
    setHeaderData(Pk3n, Qt::Vertical, tr("pKa-3"));
    setHeaderData(Pk2n, Qt::Vertical, tr("pKa-2"));
    setHeaderData(Pk1n, Qt::Vertical, tr("pKa-1"));
    setHeaderData(Pk1p, Qt::Vertical, tr("pKa+1"));
    setHeaderData(Pk2p, Qt::Vertical, tr("pKa+2"));
    setHeaderData(Pk3p, Qt::Vertical, tr("pKa+3"));
    connect(this, &QAbstractItemModel::dataChanged, this, &SegmentsModel::slotDataChanged);
}


void SegmentsModel::setSegmentsNumber(int segmentsNumber) {
    //PDEBUG << segmentsNumber;
    m_segmentsNumber = segmentsNumber;
    int currentColumnsCount = columnCount();
    if (currentColumnsCount > m_segmentsNumber) {
        int removeColumnsCount = currentColumnsCount - m_segmentsNumber;
        removeColumns(currentColumnsCount-removeColumnsCount, removeColumnsCount);
        recalculate();
    }
    if (currentColumnsCount < m_segmentsNumber) {
        int insertColumnsCount = m_segmentsNumber - currentColumnsCount;
        insertColumns(m_segmentsNumber-insertColumnsCount, insertColumnsCount);
        for (int column = m_segmentsNumber - insertColumnsCount; column < columnCount(); column++) {
            setData(index(Ratio, column), 1.0);
            setData(index(Concentration, column), 0.0);
        }
        recalculate();
        }
}


SegmentedConstituent SegmentsModel::constituent() const {
    SegmentedConstituent segments;
    double ratioSum = 0;
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        ratioSum += ratio;
    }
    for (int column = 0; column < columnCount(); column++) {
        Constituent constituent;
        auto xdataValid = [this,column](int rowU, int rowPka) {
            return data(index(rowU, column)).isValid() && data(index(rowPka, column)).isValid();
            };
        auto udata = [this,column](int row) {
            return data(index(row, column)).toDouble() * Constituent::uFactor;
            };
        auto pdata = [this,column](int row) {
            return data(index(row, column)).toDouble();
            };
        if (xdataValid(U1n,Pk1n)) { constituent.addNegU(udata(U1n)); constituent.addNegPKa(pdata(Pk1n)); }
        if (xdataValid(U2n,Pk2n)) { constituent.addNegU(udata(U2n)); constituent.addNegPKa(pdata(Pk2n)); }
        if (xdataValid(U3n,Pk3n)) { constituent.addNegU(udata(U3n)); constituent.addNegPKa(pdata(Pk3n)); }
        if (xdataValid(U1p,Pk1p)) { constituent.addPosU(udata(U1p)); constituent.addPosPKa(pdata(Pk1p)); }
        if (xdataValid(U2p,Pk2p)) { constituent.addPosU(udata(U2p)); constituent.addPosPKa(pdata(Pk2p)); }
        if (xdataValid(U3p,Pk3p)) { constituent.addPosU(udata(U3p)); constituent.addPosPKa(pdata(Pk3p)); }

        SegmentedConstituent::Segment segment;
        segment.ratio = data(index(Ratio, column)).toDouble();
        segment.concentration = data(index(Concentration, column)).toDouble();
        segment.constituent = constituent;
        segments << segment;
    }
    return segments;
}


void SegmentsModel::setConstituent(const SegmentedConstituent& segments) {
    removeColumns(0, columnCount());
    insertColumns(0, segments.size());
    for (int i=0; i<segments.size(); i++) {
        double ratio = segments.segments[i].ratio;
        double concentration = segments.segments[i].concentration;
        setData(index(Ratio, i), ratio);
        setData(index(Concentration, i), concentration);
        setData(index(Concentration, i), concentration);
        Constituent c = segments.segments[i].constituent;
        if (c.getNegCharge() >= 1) {
            setData(index(U1n,  i), c.getU  (-1) / Constituent::uFactor);
            setData(index(Pk1n, i), c.getPKa(-1));
            }
        if (c.getNegCharge() >= 2) {
            setData(index(U2n,  i), c.getU  (-2) / Constituent::uFactor);
            setData(index(Pk2n, i), c.getPKa(-2));
            }
        if (c.getNegCharge() >= 3) {
            setData(index(U3n,  i), c.getU  (-3) / Constituent::uFactor);
            setData(index(Pk3n, i), c.getPKa(-3));
            }
        if (c.getPosCharge() >= 1) {
            setData(index(U1p,  i), c.getU  (+1) / Constituent::uFactor);
            setData(index(Pk1p, i), c.getPKa(+1));
            }
        if (c.getPosCharge() >= 2) {
            setData(index(U2p,  i), c.getU  (+2) / Constituent::uFactor);
            setData(index(Pk2p, i), c.getPKa(+2));
            }
        if (c.getPosCharge() >= 3) {
            setData(index(U3p,  i), c.getU  (+3) / Constituent::uFactor);
            setData(index(Pk3p, i), c.getPKa(+3));
            }

    }
    recalculate();
}


void SegmentsModel::slotDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
    recalculate();
}


void SegmentsModel::recalculate() {
    double caplen = Simul6::instance()->getCaplen();
    double ratioSum = 0;
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        ratioSum += ratio;
    }
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        double ratioLen = (ratioSum > 0) ? 1000*ratio*caplen/ratioSum : 0;
        setHeaderData(column, Qt::Horizontal, QString("%1").arg(ratioLen, 0, 'f', 2));
    }
}
