#ifndef _MixControlTab_H_
#define _MixControlTab_H_

#include <QWidget>
#include "mixcontrolmodelabstract.h"

namespace Ui {
class MixControlTab;
}

class MixControlTab : public QWidget 
{
    Q_OBJECT

public:
    enum TabType { SwapTab, BasicTab };
    explicit MixControlTab(QWidget *parent = nullptr, TabType type = SwapTab);
    ~MixControlTab();

    const MixControlModelAbstract *model() const { return m_model; }
    void add(const QList<SegmentedConstituent>&);

    bool removable() const;

signals:
    void visibilityChanged(int id, bool visible);
    void swap(const QList<SegmentedConstituent>&);

public slots:
    void init();
    void resizeColumns();
    void hideConstituent(int internalId);
    void disableConstituent(int internalId);

private slots:
    void editComponent();
    void editComponent(const QModelIndex&);
    void removeComponent();
    void addComponent();
    void removeAll();
    void swapPressed();
    void recalculateModel();

private:
    Ui::MixControlTab *ui;

    MixControlModelAbstract *m_model;
    TabType m_tabType;
};

#endif
