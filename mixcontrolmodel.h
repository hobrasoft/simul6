#ifndef MIXCONTROLMODEL_H
#define MIXCONTROLMODEL_H

#include <QStandardItemModel>

class MixControlModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit MixControlModel(QObject *parent = nullptr);

    enum Columns {
        Name = 0,
        Color, ///< Barva látky v grafu, zvoleno v sekvenci
        NegCount,
        PosCount,
        SegCount,
        Concentrations, ///< Rozdělení koncentrací v segmentech, odděleno středníkem
        Ratio,
        LastCol
    };

private:
};

#endif // MIXCONTROLMODEL_H
