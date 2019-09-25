#ifndef SIMUL6_H
#define SIMUL6_H

#include <QMainWindow>
#include <QCloseEvent>
#include "mixcontrolmodel.h"

namespace Ui {
class Simul6;
}

class Simul6 : public QMainWindow
{
    Q_OBJECT

public:
   ~Simul6();
    static Simul6 *instance();

    const MixControlModel *mixControlModel() const;

    double getCaplen() const;

private slots:
    void init();
    void createActions();
    void closeWindow();

    void initEngine();
    void runEngine();
    void stopEngine();
    void engineFinished();
    void saveData();
    void loadData();

    void readSettings();
    void writeSettings();

private:
    explicit Simul6(QWidget *parent = nullptr);
    static Simul6 *m_instance;
    Ui::Simul6 *ui;
    void closeEvent(QCloseEvent *) override;
};

#endif // SIMUL6_H