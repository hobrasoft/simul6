/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Ampholines_H_
#define _Ampholines_H_

#include <QDialog>
#include "segmentsmodel.h"

namespace Ui {
class Ampholines;
}

/**
 * @brief
 */
class Ampholines : public QDialog {
    Q_OBJECT
public:
    Ampholines(QWidget *parent);

private slots:
    void accept();
    void recalculate();

private:
    Ui::Ampholines *ui;
    SegmentsModel *m_segmentsModel;
};

#endif

