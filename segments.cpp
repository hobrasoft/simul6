#include "segments.h"
#include "pdebug.h"

Segments::Segments() {

}

Segments::Segments(const QVariantList& json) {
    for (int i=0; i<json.size(); i++) {
        Segment segment;
        segment.concentration = json[i].toMap()["concentration"].toDouble();
        segment.ratio = json[i].toMap()["ratio"].toDouble();
        PDEBUG << segment.concentration << segment.ratio;
        segments << segment;
        }
}

