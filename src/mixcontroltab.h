#ifndef _MixControlTab_H_
#define _MixControlTab_H_

#include <QWidget>
#include <QAction>
#include <QItemSelection>
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

    void setRatios(const QList<int>& ratios);

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
    void toggleVisibility();
    void removeComponent();
    void addComponent();
    void removeAll();
    void swapPressed();
    void recalculateModel();
    void currentChanged(const QModelIndex& current, const QModelIndex& previous);
    void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    void enableActions();

private:
    Ui::MixControlTab *ui;

    MixControlModelAbstract *m_model;
    TabType m_tabType;
    QAction *m_actionSwap;
    QAction *m_actionToggleVisibility;
    QAction *m_actionAdd;
    QAction *m_actionRemove;
    QAction *m_actionEdit;
    QAction *m_actionRemoveAll;
};

#endif
