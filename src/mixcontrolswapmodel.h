#ifndef __MixControlSwapModel_H_
#define __MixControlSwapModel_H_

#include "segmentedconstituent.h"
#include "mixcontrolmodelabstract.h"


class MixControlSwapModel : public MixControlModelAbstract 
{
    Q_OBJECT

public:
    explicit MixControlSwapModel(QObject *parent = nullptr);

    enum Columns {
        Visible = 0,
        Name,
        Color, ///< Barva látky v grafu, zvoleno v sekvenci
        NegCharge,
        PosCharge,
        Concentrations, ///< Rozdělení koncentrací v segmentech, odděleno středníkem
        LastCol
    };

    void setConstituent(const SegmentedConstituent& constituent, int row) Q_DECL_OVERRIDE;

private:
};

#endif
