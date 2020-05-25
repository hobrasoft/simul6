/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Replay_H_
#define _Replay_H_

#include "ui_replay.h"
#include <QTimer>

class Engine;

/**
 * @brief
 */
class Replay : public QWidget {
    Q_OBJECT
  public:
   ~Replay();
    Replay(QWidget *);

    void setData(const QVariantList&);
    void clear();
    void setEngine(Engine *);

  private slots:
    void toBegin();
    void toEnd();
    void prevStep();
    void nextStep();
    void play();
    void setStep(int);

  private:
    Ui::Replay *ui;

    bool    m_replay;
    int     m_step;
    Engine *m_engine;
    QVariantList m_data; // item "simulation" in data file
    QTimer *m_timer;
    QAction *m_actionPlay;
};

#endif

