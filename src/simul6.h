#ifndef SIMUL6_H
#define SIMUL6_H

#include <QMainWindow>
#include <QCloseEvent>
#include "mixcontrolmodelabstract.h"
#include "crosssectionmodel.h"

namespace Ui {
class Simul6;
}

class Engine;
class Graf;
class Detector;
class ReplayDataAbstract;

class Simul6 : public QMainWindow
{
    Q_OBJECT

public:
   ~Simul6();
    static Simul6 *instance();

    const MixControlModelAbstract *mixControlModel() const;
    const CrosssectionModel *crosssectionModel() const;
    CrosssectionModel *crosssectionModel();
    
    int mixSize() const;
    Graf *graf() const;
    Detector *detector() const;
    const ReplayDataAbstract *replayData() const;

    double getCaplen() const;

    QVariantMap data() const;

    const Engine *engine() const;

    void replacedConstituent(int replacedInternalId, int replaceeInternalId);
    void hideConstituent(int internalId);
    void hideKapa();
    void hidePh();
    void hideE();

signals:
    void caplenChanged(double);


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
