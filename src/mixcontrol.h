#ifndef MIXCONTROL_H
#define MIXCONTROL_H

#include <QWidget>
#include "mixcontrolmodel.h"

namespace Ui {
class MixControl;
}

class MixControl : public QWidget 
{
    Q_OBJECT

public:
    explicit MixControl(QWidget *parent = nullptr);
    ~MixControl();

    const MixControlModel *model() const { return m_model; }

signals:
    void visibilityChanged(int id, bool visible);

public slots:
    void resizeColumns();
    void hideConstituent(int internalId);

private slots:
    void editComponent();
    void editComponent(const QModelIndex&);
    void removeComponent();
    void addComponent();
    void removeAll();

private:
    Ui::MixControl *ui;

    MixControlModel *m_model;
};

#endif // MIXCONTROL_H
