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


Segments SegmentsModel::segments() const {
    Segments segments;
    double ratioSum = 0;
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        ratioSum += ratio;
    }
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        double concentration = data(index(Concentration, column)).toDouble();
        Segments::Segment segment;
        segment.ratio = ratio;
        segment.concentration = concentration;
        segments << segment;
    }
    return segments;
}


void SegmentsModel::setSegments(const Segments& segments) {
    removeColumns(0, columnCount());
    insertColumns(0, segments.size());
    for (int i=0; i<segments.size(); i++) {
        double ratio = segments.segments[i].ratio;
        double concentration = segments.segments[i].concentration;
        setData(index(Ratio, i), ratio);
        setData(index(Concentration, i), concentration);
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
