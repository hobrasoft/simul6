#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <QList>
#include <QListIterator>
#include <QVariant>

struct Segments
{
public:
    Segments();

    struct Segment {
        double  concentration;
        double  length;
        double  ratio;
    };

    QList<Segments::Segment>	segments;
    Segments& operator<<(const Segments::Segment& segment) {
        segments << segment;
        return *this;
    }

    int size() const {
        return segments.size();
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
