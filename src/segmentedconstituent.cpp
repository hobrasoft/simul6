#include "segmentedconstituent.h"
#include "pdebug.h"

SegmentedConstituent::SegmentedConstituent() {
}

SegmentedConstituent::SegmentedConstituent(const QString& name) {
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


int SegmentedConstituent::getPosCharge() const {
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getPosCharge();
}


int SegmentedConstituent::getNegCharge() const {
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getNegCharge();
}


QVariantMap SegmentedConstituent::json() const {
    QVariantMap cdata;
    cdata["name"] = m_name;
    cdata["negCount"] = -getNegCharge();
    cdata["posCount"] =  getPosCharge();
    cdata["color"]    =  m_color;
    cdata["visible"]  =  m_visible;

    QVariantList list;
    for (int i=0; i<segments.size(); i++) {
        list << segments[i].json();
        }
    cdata["segments"] = list;

    return cdata;
}


void SegmentedConstituent::addNegPKa(double x) {
}


void SegmentedConstituent::addPosPKa(double x) {
}

void SegmentedConstituent::addNegU(double x) {
}

void SegmentedConstituent::addPosU(double x) {
}



