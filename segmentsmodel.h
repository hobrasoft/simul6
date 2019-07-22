#ifndef SEGMENTSMODEL_H
#define SEGMENTSMODEL_H

#include <QStandardItemModel>
#include "segments.h"

class SegmentsModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit SegmentsModel(QObject *parent = nullptr);

    Segments segments() const;

    enum Rows {
        Ratio = 0,
        Concentration,
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
