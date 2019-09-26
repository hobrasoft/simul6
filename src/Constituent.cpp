#include "pch.h"
#ifndef CONSTITUENT_CPP
#define CONSTITUENT_CPP

#include "Constituent.h"
#include "Engine.h"
#include <cmath>

using namespace std;

const double Constituent::uFactor = 1e-9;

Constituent::Constituent() :
    m_valid(false),
    m_id(-1)
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
    m_color = QColor(json["color"].toString());
    m_negCount = 0;
    m_posCount = 0;
    QVariantList uNeg = json["uNeg"].toList();
    QVariantList uPos = json["uPos"].toList();
    QVariantList pKaNeg = json["pKaNeg"].toList();
    QVariantList pKaPos = json["pKaPos"].toList();

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
    dbg.nospace() << '(' << c.getId() << ", " << c.getName() << ")";
    return dbg;
}


Constituent::~Constituent()
{
}

QVariantMap Constituent::json() const {
    QVariantList listuNeg;
    QVariantList listpKaNeg;
    for (unsigned int i=0; i<m_negCount; i++) {
        listuNeg.prepend(m_uNeg[i] / uFactor);
        listpKaNeg.prepend(m_pKaNeg[i]);
    }

    QVariantList listuPos;
    QVariantList listpKaPos;
    for (unsigned int i=0; i<m_posCount; i++) {
        listuPos << m_uPos[i] / uFactor;
        listpKaPos << m_pKaPos[i];
    }

//  PDEBUG << listuNeg << pKaNeg << uNeg << pKaPos << uPos;

    QVariantMap data;
    data["name"] = m_name;
    data["color"] = m_color.name(QColor::HexRgb);
    data["negCount"] = m_negCount;
    data["posCount"] = m_posCount;
    data["uNeg"] = listuNeg;
    data["uPos"] = listuPos;
    data["pKaNeg"] = listpKaNeg;
    data["pKaPos"] = listpKaPos;
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

double Constituent::getU(int pCharge) const
{
    unsigned int sign;

    if (pCharge < 0) {
        sign =static_cast<unsigned int>(-1 * pCharge - 1);
        if (sign < m_negCount) {
			return m_uNeg[sign];
		}
	}
	else {
        sign = static_cast<unsigned int>(pCharge - 1);
        if (sign < m_posCount) {
			return m_uPos[sign];
		}
	}

	return 0;
}

double Constituent::getL(const int pCharge) const
{
    if (pCharge < 0) {
        return m_lNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
    } else if (pCharge > 0) {
        return m_lPos[static_cast<unsigned int>(pCharge) - 1];
    } else {
        return 1;
    }
}

double Constituent::getPKa(const int pCharge) const
{
    if (pCharge < 0) {
        return m_pKaNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
    } else if (pCharge > 0) {
        return m_pKaPos[static_cast<unsigned int>(pCharge) - 1];
    } else {
        return 1;
    }
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
