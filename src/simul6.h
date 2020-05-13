#ifndef SIMUL6_H
#define SIMUL6_H

#include <QMainWindow>
#include <QCloseEvent>
#include "mixcontrolmodel.h"

namespace Ui {
class Simul6;
}

class Engine;
class Graf;

class Simul6 : public QMainWindow
{
    Q_OBJECT

public:
   ~Simul6();
    static Simul6 *instance();

    const MixControlModel *mixControlModel() const;
    int mixSize() const;
    Graf *graf() const;

    double getCaplen() const;

    QVariantMap data() const;

    const Engine *engine() const;

    void hideConstituent(int internalId);
    void hideKapa();
    void hidePh();
    void hideE();


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
    void setDockingWindows();

    void readSettings();
    void writeSettings();

private:
    explicit Simul6(QWidget *parent = nullptr);
    static Simul6 *m_instance;
    Ui::Simul6 *ui;
    void closeEvent(QCloseEvent *) override;
};

#endif // SIMUL6_H
