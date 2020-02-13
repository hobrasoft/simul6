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

    void setDif(const int pI, const double pValue);
    double getDif(const int pI) const;

    Sample &setIC(const unsigned int pArea, const double pValue);
    double getIC(unsigned int pArea) const;
    // A
    void setA(const int pCharge, const unsigned int pI, const double pValue);
    void setU(const int pCharge, const unsigned int pI, const double pValue);
    void setL(const int pCharge, const unsigned int pI, const double pValue);
    double getA(const int pCharge, const unsigned int pI) const;
    // D
    void setD(const int pCharge, const unsigned int pI, const double pValue);
    void addD(const int pCharge, const unsigned int pI, const double pValue);
    double getD(const int pCharge, const unsigned int pI) const;
    // V
    void initV();
    double getV(const unsigned int pI) const { return v[pI]; }
    // H
    void setH(const int pCharge, const double pValue, const unsigned int pI);
    void addH(const int pCharge, const double pValue, const unsigned int pI);
    double getH(const int pCharge, const unsigned int pI) const;
    // Hc
    void setHc(const double pValue, const unsigned int pI) { hc[pI] = pValue; }
    void addHc(const double pValue, const unsigned int pI) { hc[pI] += pValue; }
    double getHc(const unsigned int pI) const { return hc[pI]; }
    // DerH
    void setDerH(const int pCharge, const double pValue, const unsigned int pI);
    void addDerH(const int pCharge, const double pValue, const unsigned int pI);
    double getDerH(const int pCharge, const unsigned int pI) const;
    // Der Hc
    void setDerHc(const double pValue, const unsigned int pI) { derHc[pI] = pValue; }
    void addDerHc(const double pValue, const unsigned int pI) { derHc[pI] += pValue; }
    double getDerHc(const unsigned int pI) const { return derHc[pI]; }
    // Pd
    void setPd(const unsigned int pI, const double pValue) { pd[pI] = pValue; }
    double getPd(const unsigned int pI) const { return pd[pI]; }
    // q1 - q4
    void setQ1(const unsigned int pI, const double pValue) { q1[pI] = pValue; }
    void setQ2(const unsigned int pI, const double pValue) { q2[pI] = pValue; }
    void setQ3(const unsigned int pI, const double pValue) { q3[pI] = pValue; }
    void setQ4(const unsigned int pI, const double pValue) { q4[pI] = pValue; }
    void setQ5(const unsigned int pI, const double pValue) { q5[pI] = pValue; }
    void setQ6(const unsigned int pI, const double pValue) { q6[pI] = pValue; }
    double getQ1(const unsigned int pI) const { return q1[pI]; }
    double getQ2(const unsigned int pI) const { return q2[pI]; }
    double getQ3(const unsigned int pI) const { return q3[pI]; }
    double getQ4(const unsigned int pI) const { return q4[pI]; }
    double getQ5(const unsigned int pI) const { return q5[pI]; }
    double getQ6(const unsigned int pI) const { return q6[pI]; }
    // u 2d vector
    double getU(const int pCharge, const unsigned int pI) const;
    double getL(const int pCharge, const unsigned int pI) const;

};

#endif
