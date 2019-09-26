/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Ampholines_H_
#define _Ampholines_H_

#include <QDialog>
#include "segmentsmodel.h"
#include "Constituent.h"
#include "msettings.h"
#include "segments.h"

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

    const QList<Constituent>& constituents() const { return m_constituents; }
    const QList<Segments>& segments() const { return m_segments; }

private slots:
    void accept();
    void recalculate();

    void readSettings();
    void writeSettings();

private:
    Ui::Ampholines *ui;
    SegmentsModel *m_segmentsModel;

    QList<Constituent> m_constituents;
    QList<Segments> m_segments;
};

#endif

