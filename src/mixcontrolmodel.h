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

    QModelIndex add(const Constituent&, const Segments&);
    void add(const QList<Constituent>&, const QList<Segments>&);

    QList<Constituent> constituents() const;
    QList<Segments> segments() const;

    Constituent constituent(int row) const;
    Segments segments(int row) const;

    void setConstituentAndSegments(const Constituent&, const Segments&, int row);

    QVariantList json() const;
    void setJson(const QVariantList& json);

    void toggleVisible(const QModelIndex&);

signals:
    void visibilityChanged(int id, bool visible);

private:
};

#endif // MIXCONTROLMODEL_H
