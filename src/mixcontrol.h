#ifndef MIXCONTROL_H
#define MIXCONTROL_H

#include <QWidget>
#include "mixcontrolmodelabstract.h"
#include "mixcontroltab.h"

namespace Ui {
class MixControl;
}

class MixControl : public QWidget 
{
    Q_OBJECT

public:
    explicit MixControl(QWidget *parent = nullptr);
    ~MixControl();

    const MixControlModelAbstract *model() const;

signals:
    void visibilityChanged(int id, bool visible);
    void swap(const QList<SegmentedConstituent>&);

public slots:
    void init();
    void resizeColumns();
    void hideConstituent(int internalId);
    void disableConstituent(int internalId);

private slots:
    void removeCurrentTab();
    void currentTabChanged(int);

private:
    Ui::MixControl *ui;

    MixControlTab *m_basicTab;

    QAction *m_addTab;
    QAction *m_cloneTab;
    QAction *m_removeTab;
};

#endif // MIXCONTROL_H
