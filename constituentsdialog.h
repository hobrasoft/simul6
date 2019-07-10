#ifndef CONSTITUENTSDIALOG_H
#define CONSTITUENTSDIALOG_H

#include <QDialog>
#include "segmentsmodel.h"

namespace Ui {
class ConstituentsDialog;
}

class ConstituentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConstituentsDialog(QWidget *parent = nullptr);
    ~ConstituentsDialog();

private slots:
    void modelLoaded();
    void enableGroupBoxes();

private:
    Ui::ConstituentsDialog *ui;

    SegmentsModel *m_segmentsModel;
};

#endif // CONSTITUENTSDIALOG_H
