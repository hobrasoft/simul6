#ifndef MIXCONTROL_H
#define MIXCONTROL_H

#include <QGroupBox>
#include "mixcontrolmodel.h"

namespace Ui {
class MixControl;
}

class MixControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit MixControl(QWidget *parent = nullptr);
    ~MixControl();

    const MixControlModel *model() const { return m_model; }

public slots:
    void resizeColumns();

private:
    Ui::MixControl *ui;

    MixControlModel *m_model;
};

#endif // MIXCONTROL_H
