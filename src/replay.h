/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Replay_H_
#define _Replay_H_

#include "ui_replay.h"
#include "replaydataabstract.h"
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

    void setData(const QString& database_file_name);
    void setData(const QVariantList& data);
    void setMixData(const QVariantList& data);
    void setSwapsData(const QVariantList& data);
    void clear();
    void setEngine(Engine *);

    const ReplayDataAbstract *replayData() const { return m_data; }

  signals:
    void newStep();

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
    QTimer *m_timer;
    QAction *m_actionPlay;
    ReplayDataAbstract *m_data;
    void    initReplay();
    QHash<int, QVariant>    m_mixData;
};

#endif

