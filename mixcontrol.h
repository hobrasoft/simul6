#ifndef MIXCONTROL_H
#define MIXCONTROL_H

#include <QGroupBox>

namespace Ui {
class MixControl;
}

class MixControl : public QGroupBox
{
    Q_OBJECT

public:
    explicit MixControl(QWidget *parent = nullptr);
    ~MixControl();

private:
    Ui::MixControl *ui;
};

#endif // MIXCONTROL_H
