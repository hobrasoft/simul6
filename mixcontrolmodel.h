#ifndef MIXCONTROLMODEL_H
#define MIXCONTROLMODEL_H

#include <QStandardItemModel>
#include "Constituent.h"
#include "segments.h"

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

    QModelIndex add(const Constituent&, const Segments&);

    Constituent constituent(int row) const;
    Segments segments(int row) const;

    void setConstituentAndSegments(const Constituent&, const Segments&, int row);

    void save(const QString& filename) const;
    void load(const QString& filename);

private:
};

#endif // MIXCONTROLMODEL_H
