/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _GplDialog_H_
#define _GplDialog_H_

#include <QDialog>
#include "ui_gpldialog.h"

/**
 * @brief
 */
class GplDialog : public QDialog, private Ui::GplDialog {
    Q_OBJECT
  public:
    GplDialog(QWidget *parent);
};

#endif

