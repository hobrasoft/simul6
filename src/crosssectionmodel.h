/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _CrosssectionModel_H_
#define _CrosssectionModel_H_

#include <QStandardItemModel>
#include "crosssection.h"

/**
 * @brief
 */
class CrosssectionModel : public QStandardItemModel {
    Q_OBJECT
  public:
    explicit CrosssectionModel(QObject *parent = nullptr);

    enum Rows {
        Ratio = 0,
        Diameter,
        LastRow
        };

    bool unifiedDiameter() const;
    double firstSegmentDiameter() const;
    Crosssection sections() const;

  public slots:
    void recalculate();
    void setSegmentsNumber(int);
    void setDefaultDiameter(double);

  private:
    int m_segmentsNumber;
    double m_defaultDiameter;

};

#endif

