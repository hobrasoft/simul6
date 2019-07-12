#include "segmentsmodel.h"
#include "settings.h"
#include "pdebug.h"

SegmentsModel::SegmentsModel(QObject *parent)
    : QStandardItemModel(parent)
{
    insertRows(0, LastRow);
    setHeaderData(Ratio, Qt::Vertical, tr("Ratio"));
    setHeaderData(Concentration, Qt::Vertical, tr("c [mM]"));
    connect(this, &QAbstractItemModel::dataChanged, this, &SegmentsModel::slotDataChanged);
}

void SegmentsModel::setSegmentsNumber(int segmentsNumber) {
    PDEBUG << segmentsNumber;
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
            setData(index(Ratio, column), 1);
            setData(index(Concentration, column), 0);
        }
        recalculate();
        }
}


void SegmentsModel::slotDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);
    Q_UNUSED(roles);
    recalculate();
}


void SegmentsModel::recalculate() {
    double ratioSum = 0;
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        ratioSum += ratio;
    }
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        double ratioLen = (ratioSum > 0) ? 1000*ratio*CAPLEN/ratioSum : 0;
        setHeaderData(column, Qt::Horizontal, QString("%1").arg(ratioLen, 0, 'f', 2));
    }
}
