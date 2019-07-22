#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <QList>

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
};

#endif // SEGMENTS_H
