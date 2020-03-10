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
    L(CHARGE_BLOCK_SIZE, pNp),
    m_name(constituent.getName())
{
}


Sample::~Sample()
{
    //std::cout << "Sample destructor" << std::endl;
}


// V
void Sample::initV()
{
    for (int i = 0; i <= a.getNp(); i++) {
        v[i] = a.get(0, i);
    }
}


unsigned int Sample::getChargeIndex(const int pCharge) const
{
    return static_cast<unsigned int>(pCharge + static_cast<int>(Engine::maxCharge));
}


size_t Sample::getHIdx(const int pCharge, const unsigned int pI) const
{
  return pI * CHARGE_BLOCK_SIZE + getChargeIndex(pCharge);
}


