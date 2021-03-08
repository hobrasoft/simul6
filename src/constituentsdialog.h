#ifndef CONSTITUENTSDIALOG_H
#define CONSTITUENTSDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QColor>
#include <QKeyEvent>
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

    void set3FixedSegments(const QList<int>& ratios);

    SegmentedConstituent constituent() const;

    void setConstituent(const SegmentedConstituent&);
    bool manually() const;

private slots:
    void modelLoaded();
    void enableGroupBoxes();
    void currentRowChanged(int);

    void readSettings();
    void writeSettings();
    void currentDatabaseChanged(const QModelIndex& current, const QModelIndex&);

private:
    Ui::ConstituentsDialog *ui;

    int m_id;
    QColor m_color;
    SegmentsModel *m_segmentsModel;
    ConstituentsModel *m_constituentsModel;
    QSortFilterProxyModel *m_constituentsProxyModel;
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
};

#endif // CONSTITUENTSDIALOG_H
