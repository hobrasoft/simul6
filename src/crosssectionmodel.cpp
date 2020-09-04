/**
 * @file
 *
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#include "crosssectionmodel.h"
#include "simul6.h"

CrosssectionModel::CrosssectionModel(QObject *parent) : QStandardItemModel(parent) {
    m_defaultDiameter = 50;
    insertRows(0, LastRow);
    setHeaderData(Ratio,    Qt::Vertical, tr("Ratio"));
    setHeaderData(Diameter, Qt::Vertical, tr("Diameter [µm]"));
    insertColumns(0, 1);
    for (int column = 0; column < columnCount(); column++) {
        setData(index(Ratio, column), 1.0);
        setData(index(Diameter, column), m_defaultDiameter);
        }
}


Crosssection CrosssectionModel::sections() const {
    Crosssection s;
    for (int column = 0; column < columnCount(); column++) {
        double ratio = data(index(Ratio, column)).toDouble();
        double diameter = data(index(Diameter, column)).toDouble();
        s << Crosssection::Section(ratio, diameter);
        }
    return s;
}


void CrosssectionModel::setDefaultDiameter(double x) {
    m_defaultDiameter = x;
    for (int column = 0; column < columnCount(); column++) {
        setData(index(Diameter, column), m_defaultDiameter);
        }
    recalculate();
}


double CrosssectionModel::firstSegmentDiameter() const {
    return data(index(Diameter, 0)).toDouble();
}


bool CrosssectionModel::unifiedDiameter() const {
    double x = firstSegmentDiameter();
    for (int column = 0; column < columnCount(); column++) {
        if (x != data(index(Diameter,column)).toDouble()) {
            return false;
            }
        }
    return true;
}


void CrosssectionModel::setSegmentsNumber(int segmentsNumber) {
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
            QVariant ratio = data(index(Ratio, currentColumnsCount-1));
            QVariant diameter = data(index(Diameter, currentColumnsCount-1));
            setData(index(Ratio, column), ratio);
            setData(index(Diameter, column), diameter);
            }
        recalculate();
        }
}


void CrosssectionModel::recalculate() {
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

