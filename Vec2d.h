#ifndef VEC2D_H
#define VEC2D_H

#include <vector>

class Vec2d
{
private:
    unsigned int maxCharge;
    int np;
    std::vector<std::vector<double> > v;
    unsigned int getChargeIndex(const int pCharge);

public:
    Vec2d(const unsigned int pMaxCharge, const int pNp);
    unsigned int getMaxCharge() { return maxCharge; }
    int getNp() { return np; }
    double get(const int pCharge, const unsigned int pI) { return v[getChargeIndex(pCharge)][pI]; }
    void set(const int pCharge, const unsigned int pI, const double pValue) { v[getChargeIndex(pCharge)][pI] = pValue; }
};

#endif // VEC2D_H
