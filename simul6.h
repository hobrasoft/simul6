#ifndef SIMUL6_H
#define SIMUL6_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class Simul6;
}

class Simul6 : public QMainWindow
{
    Q_OBJECT

public:
    explicit Simul6(QWidget *parent = nullptr);
    ~Simul6();

private slots:
    void init();
    void createActions();
    void zavrit();

private:
    Ui::Simul6 *ui;
    void closeEvent(QCloseEvent *) override;
};

#endif // SIMUL6_H
