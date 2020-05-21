#include "segmentedconstituent.h"
#include "pdebug.h"

int SegmentedConstituent::m_lastInternalId = 1000000;
int SegmentedConstituent::m_lastId         = 1000000;

SegmentedConstituent::SegmentedConstituent() {
    m_visible = true;
    m_id = m_lastId++;
    m_internalId = m_lastInternalId++;
}


SegmentedConstituent::SegmentedConstituent(const QString& name) {
    m_name = name;
    m_visible = true;
    m_posCharge = 0;
    m_negCharge = 0;
    m_posCount = 0;
    m_negCount = 0;
    m_id = m_lastId++;
    m_internalId = m_lastInternalId++;
}


SegmentedConstituent::SegmentedConstituent(const QVariantMap& json) {
    m_name = json["name"].toString();
    m_color = QColor(json["color"].toString());
    m_visible = json.contains("visible") ? json["visible"].toBool() : true;
    m_id = json.contains("id") ? json["id"].toInt() : m_lastId++;
    m_internalId = m_lastInternalId++;

    // PDEBUG << m_name << m_color << m_visible << json["segments"];

    QVariantList segmentslist = json["segments"].toList();
    for (int i=0; i<segmentslist.size(); i++) {
        Segment segment;
        segment.concentration = segmentslist[i].toMap()["concentration"].toDouble();
        segment.ratio = segmentslist[i].toMap()["ratio"].toDouble();
        segment.constituent = Constituent(segmentslist[i].toMap()["constituent"].toMap());
        segments << segment;
        }
    updateChargesAndCounts();
}


void SegmentedConstituent::setNewInternalId() {
    m_internalId = m_lastInternalId++;
}


void SegmentedConstituent::updateChargesAndCounts() {
    if (!segments.isEmpty()) { 
        m_posCharge = segments[0].constituent.getPosCharge();
        m_posCount  = segments[0].constituent.getPosCount();
        m_negCharge = segments[0].constituent.getNegCharge();
        m_negCount  = segments[0].constituent.getNegCount();
      } else {
        m_posCharge = 0;
        m_posCount  = 0;
        m_negCharge = 0;
        m_negCount  = 0;
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
    updateChargesAndCounts();
}


void SegmentedConstituent::addPosPKa(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addPosPKa(x);
        segments[i] = s;
        }
    updateChargesAndCounts();
}


void SegmentedConstituent::addNegU(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addNegU(x);
        segments[i] = s;
        }
    updateChargesAndCounts();
}


void SegmentedConstituent::addPosU(double x) {
    for (int i=0; i<segments.size(); i++) {
        Segment s = segments[i];
        s.constituent.addPosU(x);
        segments[i] = s;
        }
    updateChargesAndCounts();
}


