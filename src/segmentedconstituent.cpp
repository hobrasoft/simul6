#include "segmentedconstituent.h"
#include "pdebug.h"

SegmentedConstituent::SegmentedConstituent() {

}

SegmentedConstituent::SegmentedConstituent(const QVariantMap& json) {
    QVariantList segmentslist = json["segments"].toList();
    for (int i=0; i<segments.size(); i++) {
        Segment segment;
        segment.concentration = segmentslist[i].toMap()["concentration"].toDouble();
        segment.ratio = segmentslist[i].toMap()["ratio"].toDouble();
        segment.constituent = Constituent(segmentslist[i].toMap()["constituent"].toMap());
        //PDEBUG << segment.concentration << segment.ratio;
        segments << segment;
        }
}

