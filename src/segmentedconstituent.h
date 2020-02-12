#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <QList>
#include <QListIterator>
#include <QVariant>
#include "Constituent.h"

class SegmentedConstituent
{
public:
    SegmentedConstituent();
    SegmentedConstituent(const QVariantMap& json);
    SegmentedConstituent(const QString& name);

    int getPosCharge() const { return m_posCharge; }
    int getNegCharge() const { return m_negCharge; }
    void setVisible(bool v) { m_visible = v; }
    bool visible() const { return m_visible; }
    void setInternalId(int x) { m_internalId = x; }
    int getInternalId() const { return m_internalId; }
    void setColor(const QColor& c) { m_color = c; }
    QColor color() const { return m_color; }
    void setName(const QString& name) { m_name = name; }
    QString getName() const { return m_name; }
    void setId(int);

    void addNegPKa(double);
    void addPosPKa(double);
    void addNegU(double);
    void addPosU(double);

    struct Segment {
        double  concentration;
        double  ratio;
        Constituent constituent;
        QVariantMap json() const {
            QVariantMap data;
            data["concentration"] = concentration;
            data["ratio"] = ratio;
            return data;
            }
    };

    QList<SegmentedConstituent::Segment>	segments;
    SegmentedConstituent& operator<<(const SegmentedConstituent::Segment& segment) {
        segments << segment;
        return *this;
    }

    int size() const {
        return segments.size();
    }

    QVariantList json() const {
        QVariantList list;
        for (int i=0; i<segments.size(); i++) {
            list << segments[i].json();
            }
        return list;
        }

    int ratioSum() const {
        int sum = 0;
        QListIterator<Segment> iterator(segments);
        while (iterator.hasNext()) {
            sum += iterator.next().ratio;
            }
        return sum;
    }

private:
    int m_posCharge;
    int m_negCharge;
    int m_internalId;
    bool m_visible;
    QColor m_color;
    QString m_name;
    
};

Q_DECLARE_METATYPE(SegmentedConstituent);

#endif // SEGMENTS_H
