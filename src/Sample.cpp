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

Sample::Sample(const SegmentedConstituent& constituent, const int pNp) :
    SegmentedConstituent(constituent),
    areas(constituent.segments.size()),
    IC(constituent.segments.size(), 0),
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
    q6(pNp + 1),
    dif(pNp + 1),
    u(CHARGE_BLOCK_SIZE, pNp),
    L(CHARGE_BLOCK_SIZE, pNp)
{
}

void Sample::setU(const int pCharge, const unsigned int i, const double value) {
    u.set(pCharge, i, value);
}


void Sample::setL(const int pCharge, const unsigned int i, const double value) {
    L.set(pCharge, i, value);
}

Sample::~Sample()
{
    //std::cout << "Sample destructor" << std::endl;
}


size_t Sample::getHIdx(const int pCharge, const unsigned int pI) const
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

double Sample::getIC(unsigned int pArea) const
{
    return (pArea < areas) ? IC[pArea] : 0;
}

unsigned int Sample::getChargeIndex(const int pCharge) const
{
    return static_cast<unsigned int>(pCharge + static_cast<int>(Engine::maxCharge));
}

// A
void Sample::setA(const int pCharge, const unsigned int pI, const double pValue)
{
    a.set(pCharge, pI, pValue);
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

double Sample::getH(const int pCharge, const unsigned int pI) const
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

double Sample::getDerH(const int pCharge, const unsigned int pI) const
{
    return derH[getHIdx(pCharge, pI)];
}

