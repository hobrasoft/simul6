#pragma once
#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include "segmentedconstituent.h"
#include "Vec2d.h"
#include <vector>
#include <string>

class Sample : public SegmentedConstituent
{
private:
    unsigned int areas;
    std::vector<double> IC;
    std::vector<double> h;
    std::vector<double> derH;
    std::vector<double> hc;
    std::vector<double> derHc;
    Vec2d a, d;
    std::vector<double> v;
    std::vector<double> pd;
    std::vector<double> q1, q2, q3, q4, q5, q6;
    std::vector<double> dif;
    Vec2d u;
    Vec2d L;

    unsigned int getChargeIndex(const int pCharge) const;

    size_t getHIdx(const int pCharge, const unsigned int pI) const;
    QString m_name;

public:
    // Sample(const unsigned int pAreas, const int pNp);
    Sample(const SegmentedConstituent& constituent, const int pNp);
    ~Sample();

    const QString& getName() const { return m_name; }
    void setName(const QString& name) { m_name = name; }

    void inline setDif(const int pI, const double pValue) { dif[pI] = pValue; }
    double inline getDif(const int pI) const { return dif[pI]; }

    Sample &setIC(const unsigned int pArea, const double pValue);
    double getIC(unsigned int pArea) const;
    // A
    void setA(const int pCharge, const unsigned int pI, const double pValue);
    void setU(const int pCharge, const unsigned int pI, const double pValue);
    void setL(const int pCharge, const unsigned int pI, const double pValue);
    double inline getA(const int pCharge, const unsigned int pI) const { return a.get(pCharge, pI); }
    // D
    void setD(const int pCharge, const unsigned int pI, const double pValue);
    void addD(const int pCharge, const unsigned int pI, const double pValue);
    double inline getD(const int pCharge, const unsigned int pI) const { return d.get(pCharge, pI); }
    // V
    void initV();
    double inline getV(const unsigned int pI) const { return v[pI]; }
    // H
    void setH(const int pCharge, const double pValue, const unsigned int pI);
    void addH(const int pCharge, const double pValue, const unsigned int pI);
    double getH(const int pCharge, const unsigned int pI) const;
    // Hc
    void inline setHc(const double pValue, const unsigned int pI) { hc[pI] = pValue; }
    void inline addHc(const double pValue, const unsigned int pI) { hc[pI] += pValue; }
    double inline getHc(const unsigned int pI) const { return hc[pI]; }
    // DerH
    void setDerH(const int pCharge, const double pValue, const unsigned int pI);
    void addDerH(const int pCharge, const double pValue, const unsigned int pI);
    double getDerH(const int pCharge, const unsigned int pI) const;
    // Der Hc
    void inline setDerHc(const double pValue, const unsigned int pI) { derHc[pI] = pValue; }
    void inline addDerHc(const double pValue, const unsigned int pI) { derHc[pI] += pValue; }
    double inline getDerHc(const unsigned int pI) const { return derHc[pI]; }
    // Pd
    void inline setPd(const unsigned int pI, const double pValue) { pd[pI] = pValue; }
    double inline getPd(const unsigned int pI) const { return pd[pI]; }
    // q1 - q4
    void inline setQ1(const unsigned int pI, const double pValue) { q1[pI] = pValue; }
    void inline setQ2(const unsigned int pI, const double pValue) { q2[pI] = pValue; }
    void inline setQ3(const unsigned int pI, const double pValue) { q3[pI] = pValue; }
    void inline setQ4(const unsigned int pI, const double pValue) { q4[pI] = pValue; }
    void inline setQ5(const unsigned int pI, const double pValue) { q5[pI] = pValue; }
    void inline setQ6(const unsigned int pI, const double pValue) { q6[pI] = pValue; }
    double inline getQ1(const unsigned int pI) const { return q1[pI]; }
    double inline getQ2(const unsigned int pI) const { return q2[pI]; }
    double inline getQ3(const unsigned int pI) const { return q3[pI]; }
    double inline getQ4(const unsigned int pI) const { return q4[pI]; }
    double inline getQ5(const unsigned int pI) const { return q5[pI]; }
    double inline getQ6(const unsigned int pI) const { return q6[pI]; }
    // u 2d vector
    double inline getU(const int pCharge, const unsigned int pI) const { return u.get(pCharge, pI); }
    double inline getL(const int pCharge, const unsigned int pI) const { return L.get(pCharge, pI); }

};

#endif
