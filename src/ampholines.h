/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Ampholines_H_
#define _Ampholines_H_

#include <QDialog>
#include <QKeyEvent>
#include "segmentsmodel.h"
#include "segmentedconstituent.h"
#include "msettings.h"

namespace Ui {
class Ampholines;
}

/**
 * @brief
 */
class Ampholines : public QDialog {
    Q_OBJECT
public:
   ~Ampholines();
    Ampholines(QWidget *parent);

    const QList<SegmentedConstituent>& constituents() const { return m_constituents; }

private slots:
    void accept();
    void recalculate();

    void readSettings();
    void writeSettings();

private:
    Ui::Ampholines *ui;
    SegmentsModel *m_segmentsModel;

    QList<SegmentedConstituent> m_constituents;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
};

#endif

