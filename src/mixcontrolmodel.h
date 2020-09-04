#ifndef MIXCONTROLMODEL_H
#define MIXCONTROLMODEL_H

#include "segmentedconstituent.h"
#include "mixcontrolmodelabstract.h"


class MixControlModel : public MixControlModelAbstract
{
    Q_OBJECT

public:
    explicit MixControlModel(QObject *parent = nullptr);

    enum Columns {
        Visible = 0,
        Name,
        Color, ///< Barva látky v grafu, zvoleno v sekvenci
        NegCharge,
        PosCharge,
        Concentrations, ///< Rozdělení koncentrací v segmentech, odděleno středníkem
        SegCount,
        Ratio,
        LastCol
    };

    void setConstituent(const SegmentedConstituent& constituent, int row) Q_DECL_OVERRIDE;

private slots:
    void recalculateCaplen(double);

private:
};

#endif // MIXCONTROLMODEL_H
