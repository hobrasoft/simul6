#include "segmentedconstituent.h"
#include "pdebug.h"


SegmentedConstituent::SegmentedConstituent() {
    m_visible = true;
}


SegmentedConstituent::SegmentedConstituent(const QString& name) {
    m_name = name;
    m_visible = true;
}


SegmentedConstituent::SegmentedConstituent(const QVariantMap& json) {
    m_name = json["name"].toString();
    m_color = QColor(json["color"].toString());
    m_visible = json.contains("visible") ? json["visible"].toBool() : true;

    PDEBUG << m_name << m_color << m_visible << json["segments"];

    QVariantList segmentslist = json["segments"].toList();
    for (int i=0; i<segmentslist.size(); i++) {
        Segment segment;
        segment.concentration = segmentslist[i].toMap()["concentration"].toDouble();
        segment.ratio = segmentslist[i].toMap()["ratio"].toDouble();
        segment.constituent = Constituent(segmentslist[i].toMap()["constituent"].toMap());
        segments << segment;
        }
}


QDebug operator<<(QDebug dbg, const SegmentedConstituent& c) {
    dbg.nospace() << '(' 
        << c.getId() << ", " << c.getName() << ", segmens count:" << c.segments.size() 
        << ", charge -/+:" << c.getNegCharge() << "/" << c.getPosCharge()
        << ")";
    for (int i=0; i<c.segments.size(); i++) {
        dbg.nospace() << " conc:" << c.segments[i].concentration << " ratio: " << c.segments[i].ratio;
        dbg.nospace() << " const: " << c.segments[i].constituent;
        }
    return dbg;
}


int SegmentedConstituent::getPosCharge() const {
    Q_ASSERT(!segments.isEmpty());
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getPosCharge();
}


int SegmentedConstituent::getNegCharge() const {
    Q_ASSERT(!segments.isEmpty());
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getNegCharge();
}


unsigned int SegmentedConstituent::getPosCount() const {
    Q_ASSERT(!segments.isEmpty());
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getPosCount();
}


unsigned int SegmentedConstituent::getNegCount() const {
    Q_ASSERT(!segments.isEmpty());
    if (segments.isEmpty()) { return 0; }
    return segments[0].constituent.getNegCount();
}




QVariantMap SegmentedConstituent::json() const {
    QVariantMap cdata;
    cdata["name"] = m_name;
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
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addNegPKa(x);
        segments[i] = s;
        }
}


void SegmentedConstituent::addPosPKa(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addPosPKa(x);
        segments[i] = s;
        }
}


void SegmentedConstituent::addNegU(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addNegU(x);
        segments[i] = s;
        }
}


void SegmentedConstituent::addPosU(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addPosU(x);
        segments[i] = s;
        }
}


void SegmentedConstituent::reserveSize(int x) {
    for (int i=0; i<x; i++) {
        segments << Segment();
        }
}


