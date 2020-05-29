/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SimulationProgressDialog_H_
#define _SimulationProgressDialog_H_

#include <QDialog>
#include "saveprogress.h"

namespace Ui {
class SimulationProgressDialog;
};


/**
 * @brief
 */
class SimulationProgressDialog : public QDialog {
    Q_OBJECT
  public:
    SimulationProgressDialog(QWidget *parent);
   ~SimulationProgressDialog();

  private slots:
    void selectFile();
    void accept();

  private:
    Ui::SimulationProgressDialog *ui;

    SaveProgress::Format m_format;
};

#endif
