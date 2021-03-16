#include "grafmju.h"
#include <math.h>
#include "constituentseries.h"


double GrafMju::value(const Sample& sample, int i, double kapa) {
    double mjuoverkapa = 0;
    for (int j = sample.getNegCharge(); j <= sample.getPosCharge(); j++) {
        if (j == 0) { continue; }
        mjuoverkapa = mjuoverkapa + sample.getA(j, i) * sample.getU(j, i) * j / abs(j); // (j>0) ? 1 : (j<0) ? -1 : 0;
        }
    if (sample.getA(0, i) > 0.001) mjuoverkapa = mjuoverkapa / sample.getA(0, i) / kapa; else mjuoverkapa = 0;
    return mjuoverkapa * 1e7;
}

QString GrafMju::valueUnit() const {
    return QStringLiteral("");
}

