#ifndef GRAFMJU_H
#define GRAFMJU_H

#include "graf.h"

QT_CHARTS_USE_NAMESPACE

class GrafMju : public Graf {
    Q_OBJECT
  public:
    GrafMju (QWidget *x) : Graf(x) {}

  public slots:
    double value(const Sample& sample, int i, double kapa, double cross) Q_DECL_OVERRIDE;
    QString valueUnit() const Q_DECL_OVERRIDE;
};

#endif // GRAFMJU_H
