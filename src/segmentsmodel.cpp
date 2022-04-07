#include "segmentsmodel.h"
#include "simul6.h"
#include "pdebug.h"

SegmentsModel::SegmentsModel(QObject *parent)
    : QStandardItemModel(parent)
{
    m_3fixedsegments = false;
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
    // PDEBUG << segmentsNumber;
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
            QVariant u3n = data(index(U3n, currentColumnsCount-1));
            QVariant u2n = data(index(U2n, currentColumnsCount-1));
            QVariant u1n = data(index(U1n, currentColumnsCount-1));
            QVariant u1p = data(index(U1p, currentColumnsCount-1));
            QVariant u2p = data(index(U2p, currentColumnsCount-1));
            QVariant u3p = data(index(U3p, currentColumnsCount-1));
            QVariant pk3n = data(index(Pk3n, currentColumnsCount-1));
            QVariant pk2n = data(index(Pk2n, currentColumnsCount-1));
            QVariant pk1n = data(index(Pk1n, currentColumnsCount-1));
            QVariant pk1p = data(index(Pk1p, currentColumnsCount-1));
            QVariant pk2p = data(index(Pk2p, currentColumnsCount-1));
            QVariant pk3p = data(index(Pk3p, currentColumnsCount-1));
            setData(index(Ratio, column), 1.0);
            setData(index(Concentration, column), 0.0);
            setData(index(U3n, column), u3n);
            setData(index(U2n, column), u2n);
            setData(index(U1n, column), u1n);
            setData(index(U1p, column), u1p);
            setData(index(U2p, column), u2p);
            setData(index(U3p, column), u3p);
            setData(index(Pk3n, column), pk3n);
            setData(index(Pk2n, column), pk2n);
            setData(index(Pk1n, column), pk1n);
            setData(index(Pk1p, column), pk1p);
            setData(index(Pk2p, column), pk2p);
            setData(index(Pk3p, column), pk3p);
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
    // PDEBUG << segments;
    removeColumns(0, columnCount());
    insertColumns(0, segments.size());
    for (int i=0; i<segments.size(); i++) {
        double ratio = segments.segments[i].ratio;
        double concentration = segments.segments[i].concentration;
        setData(index(Ratio, i), ratio);
        setData(index(Concentration, i), concentration);
        setData(index(Concentration, i), concentration);
        Constituent c = segments.segments[i].constituent;
        if (c.getNegCount() >= 1) {
            setData(index(U1n,  i), c.getU  (-1) / Constituent::uFactor);
            setData(index(Pk1n, i), c.getPKa(-1));
            }
        if (c.getNegCount() >= 2) {
            setData(index(U2n,  i), c.getU  (-2) / Constituent::uFactor);
            setData(index(Pk2n, i), c.getPKa(-2));
            }
        if (c.getNegCount() >= 3) {
            setData(index(U3n,  i), c.getU  (-3) / Constituent::uFactor);
            setData(index(Pk3n, i), c.getPKa(-3));
            }
        if (c.getPosCount() >= 1) {
            setData(index(U1p,  i), c.getU  (+1) / Constituent::uFactor);
            setData(index(Pk1p, i), c.getPKa(+1));
            }
        if (c.getPosCount() >= 2) {
            setData(index(U2p,  i), c.getU  (+2) / Constituent::uFactor);
            setData(index(Pk2p, i), c.getPKa(+2));
            }
        if (c.getPosCount() >= 3) {
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


void SegmentsModel::setRatios(const QList<int>& ratios) {
    Q_ASSERT(ratios.size() == columnCount());
    for (int i = 0; i<ratios.size(); i++) {
        setData(index(Ratio, i), ratios[i]);
        }
    recalculate();
}


void SegmentsModel::recalculate() {
    blockSignals(true);
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
        if (m_3fixedsegments) {
            QVariant u3n = data(index(U3n, 1));
            QVariant u2n = data(index(U2n, 1));
            QVariant u1n = data(index(U1n, 1));
            QVariant u1p = data(index(U1p, 1));
            QVariant u2p = data(index(U2p, 1));
            QVariant u3p = data(index(U3p, 1));
            QVariant pk3n = data(index(Pk3n, 1));
            QVariant pk2n = data(index(Pk2n, 1));
            QVariant pk1n = data(index(Pk1n, 1));
            QVariant pk1p = data(index(Pk1p, 1));
            QVariant pk2p = data(index(Pk2p, 1));
            QVariant pk3p = data(index(Pk3p, 1));
            setData(index(Concentration, 0), 0.0);
            setData(index(Concentration, 2), 0.0);
            setData(index(U3n, 0), u3n);
            setData(index(U2n, 0), u2n);
            setData(index(U1n, 0), u1n);
            setData(index(U1p, 0), u1p);
            setData(index(U2p, 0), u2p);
            setData(index(U3p, 0), u3p);
            setData(index(Pk3n, 0), pk3n);
            setData(index(Pk2n, 0), pk2n);
            setData(index(Pk1n, 0), pk1n);
            setData(index(Pk1p, 0), pk1p);
            setData(index(Pk2p, 0), pk2p);
            setData(index(Pk3p, 0), pk3p);
            setData(index(U3n, 2), u3n);
            setData(index(U2n, 2), u2n);
            setData(index(U1n, 2), u1n);
            setData(index(U1p, 2), u1p);
            setData(index(U2p, 2), u2p);
            setData(index(U3p, 2), u3p);
            setData(index(Pk3n, 2), pk3n);
            setData(index(Pk2n, 2), pk2n);
            setData(index(Pk1n, 2), pk1n);
            setData(index(Pk1p, 2), pk1p);
            setData(index(Pk2p, 2), pk2p);
            setData(index(Pk3p, 2), pk3p);
            }
    }
    blockSignals(false);
}




