#include "pch.h"
#ifndef CONSTITUENT_CPP
#define CONSTITUENT_CPP

#include "Constituent.h"
#include "Engine.h"
#include "pdebug.h"
#include <cmath>

using namespace std;

const double Constituent::uFactor = 1e-9;

Constituent::Constituent() :
    m_valid(false),
    m_id(-1),
	m_negCount(0),
    m_posCount(0)
{
}

Constituent::Constituent(const QString& pName, const double pU, const int pZ) :
    m_valid(true),
    m_id(-1),
	m_name(pName), 
	m_U(pU), 
	m_Z(pZ),
	m_negCount(0),
    m_posCount(0),
    m_dif(0)
{
}


Constituent::Constituent(const QVariantMap& json) {
    m_id = -1;
    m_U = 0; m_Z = 0;
    m_valid = true;
    m_name = json["name"].toString();
    // m_color = QColor(json["color"].toString());
    // m_visible = json.contains("visible") ? json["visible"].toBool() : true;
    m_negCount = 0;
    m_posCount = 0;
    QVariantList uNeg = json["uNeg"].toList();
    QVariantList uPos = json["uPos"].toList();
    QVariantList pKaNeg = json["pKaNeg"].toList();
    QVariantList pKaPos = json["pKaPos"].toList();

    // m_posCount = uPos.size();
    // m_negCount = uNeg.size();

    QListIterator<QVariant> citerator(uNeg);
    for (citerator = uNeg, citerator.toBack(); citerator.hasPrevious();) {
        addNegU(citerator.previous().toDouble() * Constituent::uFactor);
        }
    for (citerator = uPos; citerator.hasNext();) {
        addPosU(citerator.next().toDouble() * Constituent::uFactor);
        }
    for (citerator = pKaNeg, citerator.toBack(); citerator.hasPrevious();) {
        addNegPKa(citerator.previous().toDouble());
        }
    for (citerator = pKaPos; citerator.hasNext();) {
        addPosPKa(citerator.next().toDouble());
        }

}


QDebug operator<<(QDebug dbg, const Constituent& c) {
    QStringList p, n;
    for (unsigned int i=0; i<c.m_posCount; i++) {
        p << QString("%1").arg(c.m_uPos[i]);
        }
    for (unsigned int i=0; i<c.m_negCount; i++) {
        n << QString("%1").arg(c.m_uNeg[i]);
        }
    dbg.nospace() << '(' << c.getId() << ", " << c.getName() << p << n << ")";
    return dbg;
}


Constituent::~Constituent()
{
}

QVariantMap Constituent::json() const {
    QVariantMap data = jsonSimplified();
    data["name"] = m_name;
    data["negCount"] = m_negCount;
    data["posCount"] = m_posCount;
    return data;
}


QVariantMap Constituent::jsonSimplified() const {
    QVariantList listuNeg;
    QVariantList listpKaNeg;
    for (unsigned int i=0; i<m_negCount; i++) {
        listuNeg.prepend(m_uNeg[i] / Constituent::uFactor);
        listpKaNeg.prepend(m_pKaNeg[i]);
    }

    QVariantList listuPos;
    QVariantList listpKaPos;
    for (unsigned int i=0; i<m_posCount; i++) {
        listuPos << m_uPos[i] / Constituent::uFactor;
        listpKaPos << m_pKaPos[i];
    }

    QVariantMap data;
    if (!listuNeg.isEmpty()) { data["uNeg"] = listuNeg; }
    if (!listuPos.isEmpty()) { data["uPos"] = listuPos; }
    if (!listpKaNeg.isEmpty()) { data["pKaNeg"] = listpKaNeg; }
    if (!listpKaPos.isEmpty()) { data["pKaPos"] = listpKaPos; }
    return data;
}


void Constituent::addNegU(double pValue)
{
	m_uNeg.push_back(pValue);
	m_negCount++;
    calculateDif();
}


void Constituent::addPosU(double pValue)
{
	m_uPos.push_back(pValue);
	m_posCount++;
    calculateDif();
}

void Constituent::addNegPKa(double pValue)
{
    size_t c = m_pKaNeg.size();

	m_pKaNeg.push_back(pValue);
    m_kaNeg.push_back(exp(-pValue * log(10)));
    m_lNeg.push_back(((c == 0) ? 1 : m_lNeg[c - 1]) * m_kaNeg[c]);
}

void Constituent::addPosPKa(double pValue)
{
    size_t c = m_pKaPos.size();

	m_pKaPos.push_back(pValue);
    m_kaPos.push_back(exp(-pValue * log(10)));
    m_lPos.push_back(((c == 0) ? 1 : m_lPos[c - 1]) / m_kaPos[c]);
}


void Constituent::calculateDif()
{
    if (m_posCount > 0) {
        m_dif = getU(1) * Engine::r * Engine::te / Engine::farc;
    } else {
        if (m_negCount > 0) {
            m_dif = getU(-1) * Engine::r * Engine::te / Engine::farc;
        }
    }
}


#endif 
