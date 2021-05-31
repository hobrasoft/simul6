#include "graflconc.h"
#include <math.h>
#include "constituentseries.h"


double GrafLConc::value(const Sample& sample, int i, double kapa, double cross) {
    Q_UNUSED(kapa);
    return sample.getA(0, i) * cross * 1e9;
}

QString GrafLConc::valueUnit() const {
    return QStringLiteral("10⁹·mol/m");
}

