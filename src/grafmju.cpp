#include "grafmju.h"
#include <math.h>
#include "constituentseries.h"


double GrafMju::value(const Sample& sample, int i, double kapa, double cross) {
    Q_UNUSED (cross);
    double mjuoverkapa = 0;
    for (int j = sample.getNegCharge(); j <= sample.getPosCharge(); j++) {
        if (j == 0) { continue; }
        mjuoverkapa = mjuoverkapa + sample.getA(j, i) * sample.getU(j, i) * j / abs(j); // (j>0) ? 1 : (j<0) ? -1 : 0;
        }
    if (sample.getA(0, i) > 0.1) mjuoverkapa = mjuoverkapa / sample.getA(0, i) / kapa; else mjuoverkapa = 0;
    return mjuoverkapa * 1e9;
}

QString GrafMju::valueUnit() const {
    return QStringLiteral("");
}

