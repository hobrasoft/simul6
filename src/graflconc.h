#ifndef GRAFLCONC_H
#define GRAFLCONC_H

#include "graf.h"

QT_CHARTS_USE_NAMESPACE

class GrafLConc : public Graf {
    Q_OBJECT
  public:
    GrafLConc (QWidget *x) : Graf(x) {}

  public slots:
    double value(const Sample& sample, int i, double kapa, double cross) Q_DECL_OVERRIDE;
    QString valueUnit() const Q_DECL_OVERRIDE;
};

#endif
