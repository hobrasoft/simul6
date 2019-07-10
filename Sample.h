#pragma once
#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include "Constituent.h"
#include "Vec2d.h"
#include <vector>
#include <string>

class Sample : public Constituent
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
    std::vector<double> q1, q2, q3, q4;

    unsigned int getChargeIndex(const int pCharge);

    size_t getHIdx(const int pCharge, const unsigned int pI);

public:
    Sample(const Constituent &pConstituent, const unsigned int pAreas, const int pNp);
    ~Sample();

    Sample &setIC(const unsigned int pArea, const double pValue);
    double getIC(unsigned int pArea);
    // A
    void setA(const int pCharge, const unsigned int pI, const double pValue);
    double getA(const int pCharge, const unsigned int pI);
    // D
    void setD(const int pCharge, const unsigned int pI, const double pValue);
    void addD(const int pCharge, const unsigned int pI, const double pValue);
    double getD(const int pCharge, const unsigned int pI);
    // V
    void initV();
    double getV(const unsigned int pI) { return v[pI]; }
    // H
    void setH(const int pCharge, const double pValue, const unsigned int pI);
    void addH(const int pCharge, const double pValue, const unsigned int pI);
    double getH(const int pCharge, const unsigned int pI);
    // Hc
    void setHc(const double pValue, const unsigned int pI) { hc[pI] = pValue; }
    void addHc(const double pValue, const unsigned int pI) { hc[pI] += pValue; }
    double getHc(const unsigned int pI) { return hc[pI]; }
    // DerH
    void setDerH(const int pCharge, const double pValue, const unsigned int pI);
    void addDerH(const int pCharge, const double pValue, const unsigned int pI);
    double getDerH(const int pCharge, const unsigned int pI);
    // Der Hc
    void setDerHc(const double pValue, const unsigned int pI) { derHc[pI] = pValue; }
    void addDerHc(const double pValue, const unsigned int pI) { derHc[pI] += pValue; }
    double getDerHc(const unsigned int pI) { return derHc[pI]; }
    // Pd
    void setPd(const unsigned int pI, const double pValue) { pd[pI] = pValue; }
    double getPd(const unsigned int pI) { return pd[pI]; }
    // q1 - q4
    void setQ1(const unsigned int pI, const double pValue) { q1[pI] = pValue; }
    void setQ2(const unsigned int pI, const double pValue) { q2[pI] = pValue; }
    void setQ3(const unsigned int pI, const double pValue) { q3[pI] = pValue; }
    void setQ4(const unsigned int pI, const double pValue) { q4[pI] = pValue; }
    double getQ1(const unsigned int pI) { return q1[pI]; }
    double getQ2(const unsigned int pI) { return q2[pI]; }
    double getQ3(const unsigned int pI) { return q3[pI]; }
    double getQ4(const unsigned int pI) { return q4[pI]; }

    void show();
};

#endif
