#ifndef SEGMENTSMODEL_H
#define SEGMENTSMODEL_H

#include <QStandardItemModel>
#include "segmentedconstituent.h"

class SegmentsModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit SegmentsModel(QObject *parent = nullptr);

    SegmentedConstituent constituent() const;
    void setConstituent(const SegmentedConstituent&);
    void setRatios(const QList<int>& ratios);

    enum Rows {
        Ratio = 0,
        Concentration,
        U3n,
        U2n,
        U1n,
        U1p,
        U2p,
        U3p,
        Pk3n,
        Pk2n,
        Pk1n,
        Pk1p,
        Pk2p,
        Pk3p,
        LastRow
    };

public slots:
    void recalculate();
    void setSegmentsNumber(int);

private slots:
    void slotDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&);

private:
    int m_segmentsNumber;
};

#endif // SEGMENTSMODEL_H
