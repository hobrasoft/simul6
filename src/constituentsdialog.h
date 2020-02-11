#ifndef CONSTITUENTSDIALOG_H
#define CONSTITUENTSDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QColor>
#include "segmentsmodel.h"
#include "constituentsmodel.h"
#include "segmentedconstituent.h"
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

    SegmentedConstituent constituent() const;

    void setConstituent(const SegmentedConstituent&);
//  void setConstituent(const Constituent&);
    bool manually() const;

private slots:
    void modelLoaded();
    void enableGroupBoxes();
    void currentRowChanged(int);

    void readSettings();
    void writeSettings();

private:
    Ui::ConstituentsDialog *ui;

    int m_id;
    QColor m_color;
    SegmentsModel *m_segmentsModel;
//  ParametersModel *m_parametersModel;
    ConstituentsModel *m_constituentsModel;
    QSortFilterProxyModel *m_constituentsProxyModel;
};

#endif // CONSTITUENTSDIALOG_H
