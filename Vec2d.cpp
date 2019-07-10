#include "Vec2d.h"

Vec2d::Vec2d(const unsigned int pMaxCharge, const int pNp) :
    maxCharge(pMaxCharge),
    np(pNp),
    v(2 * maxCharge + 1, std::vector<double>(pNp + 1, 0))
{
}

unsigned int Vec2d::getChargeIndex(const int pCharge)
{
    return static_cast<unsigned int>(pCharge + static_cast<int>(maxCharge));
}
