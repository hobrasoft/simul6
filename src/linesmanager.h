/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _LinesManager_H_
#define _LinesManager_H_

#include <QDialog>
#include "linesmanager.h"
#include "linesmodel.h"

namespace Ui {
class LinesManager;
}


/**
 * @brief
 */
class LinesManager : public QDialog {
    Q_OBJECT
  public:
    explicit LinesManager(QWidget *parent = nullptr);

  private slots:
    void    add();
    void    remove();

  private:
    Ui::LinesManager *ui;
    LinesModel *model() const;


};

#endif

