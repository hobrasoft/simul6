#ifndef __MixControlModelAbstract_H_
#define __MixControlModelAbstract_H_

#include <QStandardItemModel>
#include "segmentedconstituent.h"


class MixControlModelAbstract : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit MixControlModelAbstract(QObject *parent = nullptr);

    QModelIndex add(const SegmentedConstituent&);
    void add(const QList<SegmentedConstituent>&);
    void add(const QList<Constituent>&);

    QList<SegmentedConstituent> constituents() const;
    SegmentedConstituent constituent(int row) const;

    virtual void setConstituent(const SegmentedConstituent&, int row) = 0;

    void toggleVisible(const QModelIndex&);
    void hide(int internalId);

    QVariantList json() const;
    void setJson(const QVariantList& json);

signals:
    void visibilityChanged(int id, bool visible);

private:
};

#endif
