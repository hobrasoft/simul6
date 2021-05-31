/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _ExportProgressCsv_H_
#define _ExportProgressCsv_H_

#include <QDialog>
#include "replaydataabstract.h"
#include "Engine.h"

namespace Ui {
class ExportProgressCsv;
}

/**
 * @brief
 */
class ExportProgressCsv : public QDialog {
    Q_OBJECT
  public:
    ExportProgressCsv(QWidget *parent, int np);
   ~ExportProgressCsv();

    void setReplayData(const ReplayDataAbstract *data) { m_data = data; }
    void setMixData   (const QHash<int, QVariant>& mixData) { m_mixData = mixData; }
    void setErrH(double x) { m_engine->setErrH(x); }
    void setCapLen(double x) { m_engine->setCapLen(x); }
    void setBW(int x) { m_engine->setBW(x); }
    void setTimeInterval(double x) { m_engine->setTimeInterval(x); }
    void setTimeStop(double x) { m_engine->setTimeStop(x); }
    void setDt(double x) { m_engine->setDt(x); }
    void setVoltage(double x) { m_engine->setVoltage(x); }
    void setCurDen(double x) { m_engine->setCurDen(x); }
    void setConstantVoltage(bool x) { m_engine->setConstantVoltage(x); }
    void setOptimizeDt(bool x) { m_engine->setOptimizeDt(x); }
    void setMix(const QList<SegmentedConstituent>& x) { m_engine->setMix(x); }
    void setCrosssection(const Crosssection& x) { m_engine->setCrosssection(x); }
    void setDetectorActive(bool x) { m_engine->setDetectorActive(x); }


  private slots:
    void selectFile();
    void accept() Q_DECL_OVERRIDE;

  private:
    Ui::ExportProgressCsv *ui;
    QString m_filename;
    const ReplayDataAbstract *m_data;
    QHash<int, QVariant> m_mixData;
    Engine *m_engine;
    void saveCsv();
};

#endif

