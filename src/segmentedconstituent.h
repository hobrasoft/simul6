#ifndef _SegmentedConstituent_H_
#define _SegmentedConstituent_H_

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

    int getPosCharge() const;
    int getNegCharge() const;
    unsigned int getPosCount() const;
    unsigned int getNegCount() const;
    void setVisible(bool v) { m_visible = v; }
    bool visible() const { return m_visible; }
    void setInternalId(int x) { m_internalId = x; }
    int getInternalId() const { return m_internalId; }
    void setColor(const QColor& c) { m_color = c; }
    QColor color() const { return m_color; }
    void setName(const QString& name) { m_name = name; }
    QString getName() const { return m_name; }
    void setId(int x) { m_id = x; } 
    int getId() const { return m_id; }

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
            data["constituent"] = constituent.jsonSimplified();
            return data;
            }
    };

    QList<SegmentedConstituent::Segment> segments;
    SegmentedConstituent& operator<<(const SegmentedConstituent::Segment& segment) {
        segments << segment;
        return *this;
    }

    int size() const {
        return segments.size();
    }

    void reserveSize(int);

    QVariantMap json() const;

    int ratioSum() const {
        int sum = 0;
        QListIterator<Segment> iterator(segments);
        while (iterator.hasNext()) {
            sum += iterator.next().ratio;
            }
        return sum;
    }

private:
    int m_internalId;
    bool m_visible;
    QColor m_color;
    QString m_name;
    int m_id;
    
};

QDebug operator<<(QDebug dbg, const SegmentedConstituent& other);

Q_DECLARE_METATYPE(SegmentedConstituent);

#endif
