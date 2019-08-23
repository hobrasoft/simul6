#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <QList>
#include <QListIterator>
#include <QVariant>

struct Segments
{
public:
    Segments();
    Segments(const QVariantList& json);

    struct Segment {
        double  concentration;
        double  ratio;
        QVariantMap json() const {
            QVariantMap data;
            data["concentration"] = concentration;
            data["ratio"] = ratio;
            return data;
            }
    };

    QList<Segments::Segment>	segments;
    Segments& operator<<(const Segments::Segment& segment) {
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
};

Q_DECLARE_METATYPE(Segments);

#endif // SEGMENTS_H
