#include "pch.h"
#ifndef CONSTITUENT_CPP
#define CONSTITUENT_CPP

#include "Constituent.h"
#include "Engine.h"
#include <iostream>
#include <cmath>

using namespace std;

Constituent::Constituent()
:	valid(false)
{
}

Constituent::Constituent(const std::string pName, const double pU, const int pZ) :
    valid(true),
	name(pName), 
	U(pU), 
	Z(pZ),
	negCount(0),
    posCount(0),
    dif(0)
{
	cout << "Constituent " << pName << " constructor" << endl;
}


/*Constituent::Constituent(const Constituent &pConstituent)
{
	name = pConstituent.name;
	U = pConstituent.U;
	Z = pConstituent.Z;
	negCount = pConstituent.negCount;
	posCount = pConstituent.posCount;
}
*/

Constituent::~Constituent()
{
	cout << "Constituent destructor" << endl;
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

double Constituent::getU(int pCharge)
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

double Constituent::getL(const int pCharge)
{
    if (pCharge < 0) {
        return lNeg[static_cast<unsigned int>(-1 * pCharge) - 1];
    } else if (pCharge > 0) {
        return lPos[static_cast<unsigned int>(pCharge) - 1];
    } else {
        return 1;
    }
}

double Constituent::getPKa(const int pCharge)
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

void Constituent::show()
{
	int i = 0;
	cout << "Showing constituent" << endl;
	cout << "- name: " << name << endl;
	cout << "- U, Z: " << U << ", " << Z << endl;
	cout << "- negCount: " << negCount << endl;
	cout << "- posCount: " << posCount << endl;

	cout << "- U-: ";
	for (auto &j : uNeg) {
		if (i++ > 0) {
			cout << ", ";
		}
		cout << j;
	}
	cout << endl;

	cout << "- U+: ";
	i = 0;
	for (auto &j : uPos) {
		if (i++ > 0) {
			cout << ", ";
		}
		cout << j;
	}
	cout << endl;

	cout << "- pKa-: ";
	for (auto &j : pKaNeg) {
		if (i++ > 0) {
			cout << ", ";
		}
		cout << j;
	}
	cout << endl;

	cout << "- pKa+: ";
	i = 0;
	for (auto &j : pKaPos) {
		if (i++ > 0) {
			cout << ", ";
		}
		cout << j;
	}
	cout << endl;

    cout << "- L: ";
    for (int i = static_cast<int>(-negCount); i <= static_cast<int>(posCount); i++) {
        if (i != static_cast<int>(-negCount)) {
            cout << ", ";
        }
        cout << "(" << i << ") = " << getL(i);
    }
    cout << endl;

    cout << "difK: " << dif << endl;
}

#endif 
