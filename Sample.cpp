#include "pch.h"

#include "Sample.h"
#include "Engine.h"
#include <iostream>

static const unsigned int CHARGE_BLOCK_SIZE{Engine::maxCharge * 2 + 1};

inline
std::vector<double> initHVector(const unsigned int N)
{
  std::vector<double> vec{};

  vec.resize(N * CHARGE_BLOCK_SIZE, 0);

  return vec;
}

Sample::Sample(const Constituent &pConstituent, const unsigned int pAreas, const int pNp) :
    Constituent(pConstituent),
    areas(pAreas),
    IC(pAreas, 0),
    h(initHVector(pNp + 1)),
    derH(initHVector(pNp + 1)),
    hc(pNp + 1, 0),
    derHc(pNp + 1, 0),
    a(CHARGE_BLOCK_SIZE, pNp),
    d(CHARGE_BLOCK_SIZE, pNp),
    v(pNp + 1),
    pd(pNp + 1),
    q1(pNp + 1),
    q2(pNp + 1),
    q3(pNp + 1),
    q4(pNp + 1),
    q5(pNp + 1),
    q6(pNp + 1)
{
    std::cout << "Sample constructor" << std::endl;
}

Sample::~Sample()
{
    std::cout << "Sample destructor" << std::endl;
}

size_t Sample::getHIdx(const int pCharge, const unsigned int pI)
{
  return pI * CHARGE_BLOCK_SIZE + getChargeIndex(pCharge);
}

Sample &Sample::setIC(const unsigned int pArea, const double pValue)
{
    if (pArea < areas) {
        IC[pArea] = pValue;
    }

    return *this;
}

double Sample::getIC(unsigned int pArea)
{
    return (pArea < areas) ? IC[pArea] : 0;
}

unsigned int Sample::getChargeIndex(const int pCharge)
{
    return static_cast<unsigned int>(pCharge + static_cast<int>(Engine::maxCharge));
}

// A
void Sample::setA(const int pCharge, const unsigned int pI, const double pValue)
{
    a.set(pCharge, pI, pValue);
}

double Sample::getA(const int pCharge, const unsigned int pI)
{
    return a.get(pCharge, pI);
}

// D
void Sample::setD(const int pCharge, const unsigned int pI, const double pValue)
{
    d.set(pCharge, pI, pValue);
}

void Sample::addD(const int pCharge, const unsigned int pI, const double pValue)
{
    setD(pCharge, pI, getD(pCharge, pI) + pValue);
}

double Sample::getD(const int pCharge, const unsigned int pI)
{
    return d.get(pCharge, pI);
}

// V
void Sample::initV()
{
    for (int i = 0; i <= a.getNp(); i++) {
        v[i] = a.get(0, i);
    }
}

// H
void Sample::setH(const int pCharge, const double pValue, const unsigned int pI)
{
    h[getHIdx(pCharge, pI)] = pValue;
}

void Sample::addH(const int pCharge, const double pValue, const unsigned int pI)
{
    h[getHIdx(pCharge, pI)] += pValue;
}

double Sample::getH(const int pCharge, const unsigned int pI)
{
    return h[getHIdx(pCharge, pI)];
}

// der H
void Sample::setDerH(const int pCharge, const double pValue, const unsigned int pI)
{
    derH[getHIdx(pCharge, pI)] = pValue;
}

void Sample::addDerH(const int pCharge, const double pValue, const unsigned int pI)
{
    derH[getHIdx(pCharge, pI)] += pValue;
}

double Sample::getDerH(const int pCharge, const unsigned int pI)
{
    return derH[getHIdx(pCharge, pI)];
}

void Sample::show()
{
    std::cout << "Showing sample" << std::endl;
    Constituent::show();

    for (unsigned int i = 0; i < areas; i++) {
        std::cout << "- IC[" << i << "] = " << IC[i] << std::endl;
    }
}
