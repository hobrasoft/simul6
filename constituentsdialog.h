#ifndef CONSTITUENTSDIALOG_H
#define CONSTITUENTSDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include "segmentsmodel.h"
#include "parametersmodel.h"
#include "constituentsmodel.h"
#include "segments.h"
#include "Constituent.h"

namespace Ui {
class ConstituentsDialog;
}

class ConstituentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConstituentsDialog(QWidget *parent = nullptr);
    ~ConstituentsDialog();

    Constituent constituent() const;
    Segments segments() const;

private slots:
    void modelLoaded();
    void enableGroupBoxes();
    void currentRowChanged(int);

private:
    Ui::ConstituentsDialog *ui;

    int m_id;
    SegmentsModel *m_segmentsModel;
    ParametersModel *m_parametersModel;
    ConstituentsModel *m_constituentsModel;
    QSortFilterProxyModel *m_constituentsProxyModel;
};

#endif // CONSTITUENTSDIALOG_H
