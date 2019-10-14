/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _SimulationProgressDialog_H_
#define _SimulationProgressDialog_H_

#include <QDialog>

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

  private:
    Ui::SimulationProgressDialog *ui;
};

#endif
