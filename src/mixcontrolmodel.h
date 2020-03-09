#ifndef MIXCONTROLMODEL_H
#define MIXCONTROLMODEL_H

#include <QStandardItemModel>
#include "segmentedconstituent.h"


class MixControlModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit MixControlModel(QObject *parent = nullptr);

    enum Columns {
        Visible = 0,
        Name,
        Color, ///< Barva látky v grafu, zvoleno v sekvenci
        NegCount,
        PosCount,
        Concentrations, ///< Rozdělení koncentrací v segmentech, odděleno středníkem
        SegCount,
        Ratio,
        LastCol
    };

    QModelIndex add(const SegmentedConstituent&);
    void add(const QList<SegmentedConstituent>&);
    void add(const QList<Constituent>&);

    QList<SegmentedConstituent> constituents() const;
    SegmentedConstituent constituent(int row) const;

    void setConstituent(const SegmentedConstituent&, int row);

    QVariantList json() const;
    void setJson(const QVariantList& json);

    void toggleVisible(const QModelIndex&);

signals:
    void visibilityChanged(int id, bool visible);

private:
};

#endif // MIXCONTROLMODEL_H
