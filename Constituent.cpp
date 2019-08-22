#include "pch.h"
#ifndef CONSTITUENT_CPP
#define CONSTITUENT_CPP

#include "Constituent.h"
#include "Engine.h"
#include <cmath>

using namespace std;

const double Constituent::uFactor = 1e-9;

Constituent::Constituent()
:	valid(false)
{
}

Constituent::Constituent(const QString& pName, const double pU, const int pZ) :
    valid(true),
	name(pName), 
	U(pU), 
	Z(pZ),
	negCount(0),
    posCount(0),
    dif(0)
{
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
    for (unsigned int i=0; i<negCount; i++) {
        listuNeg.prepend(uNeg[i] / uFactor);
        listpKaNeg.prepend(pKaNeg[i]);
    }

    QVariantList listuPos;
    QVariantList listpKaPos;
    for (unsigned int i=0; i<posCount; i++) {
        listuNeg << uPos[i] / uFactor;
        listpKaNeg << pKaPos[i];
    }

    QVariantMap data;
    data["name"] = name;
    data["negCount"] = negCount;
    data["posCount"] = posCount;
    data["uNeg"] = listuNeg;
    data["uPos"] = listuPos;
    data["pKaNeg"] = listpKaNeg;
    data["pKaPos"] = listpKaPos;
    return data;
}

void Constituent::addNegU(double pValue)
{
	uNeg.push_back(pValue);
	negCount++;
    calculateDif();
}

void Constituent::addPosU(double pValue)
{
	uPos.push_back(pValue);
	posCount++;
    calculateDif();
}

void Constituent::addNegPKa(double pValue)
{
    size_t c = pKaNeg.size();

	pKaNeg.push_back(pValue);
    kaNeg.push_back(exp(-pValue * log(10)));
    lNeg.push_back(((c == 0) ? 1 : lNeg[c - 1]) * kaNeg[c]);
}

void Constituent::addPosPKa(double pValue)
{
    size_t c = pKaPos.size();

	pKaPos.push_back(pValue);
    kaPos.push_back(exp(-pValue * log(10)));
    lPos.push_back(((c == 0) ? 1 : lPos[c - 1]) / kaPos[c]);
}

double Constituent::getU(int pCharge) const
{
    unsigned int sign;

    if (pCharge < 0) {
        sign =static_cast<unsigned int>(-1 * pCharge - 1);
        if (sign < negCount) {
			return uNeg[sign];
		}
	}
	else {
        sign = static_cast<unsigned int>(pCharge - 1);
        if (sign < posCount) {
			return uPos[sign];
		}
	}

	return 0;
}

double Constituent::getL(const int pCharge) const
{
    if (pCharge < 0) {
        return lNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
    } else if (pCharge > 0) {
        return lPos[static_cast<unsigned int>(pCharge) - 1];
    } else {
        return 1;
    }
}

double Constituent::getPKa(const int pCharge) const
{
    if (pCharge < 0) {
        return pKaNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
    } else if (pCharge > 0) {
        return pKaPos[static_cast<unsigned int>(pCharge) - 1];
    } else {
        return 1;
    }
}

void Constituent::calculateDif()
{
    if (posCount > 0) {
        dif = getU(1) * Engine::r * Engine::te / Engine::farc;
    } else {
        if (negCount > 0) {
            dif = getU(-1) * Engine::r * Engine::te / Engine::farc;
        }
    }
}


#endif 
