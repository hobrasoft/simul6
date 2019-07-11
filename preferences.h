#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = nullptr);
    ~Preferences();

public slots:
    void accept() override;

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
