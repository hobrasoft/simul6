#ifndef CONSTITUENTSDIALOG_H
#define CONSTITUENTSDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QColor>
#include "crosssectionmodel.h"

namespace Ui {
class CrosssectionDialog;
}

class CrosssectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrosssectionDialog(QWidget *parent = nullptr);
    ~CrosssectionDialog();

private slots:
    void readSettings();
    void writeSettings();

    void accept();
    void reject();

private:
    Ui::CrosssectionDialog *ui;
    CrosssectionModel *m_model;

};

#endif // CONSTITUENTSDIALOG_H
