/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Replay_H_
#define _Replay_H_

#include "ui_replay.h"

/**
 * @brief
 */
class Replay : public QWidget {
    Q_OBJECT
  public:
   ~Replay();
    Replay(QWidget *);

  private:
    Ui::Replay *ui;
};

#endif

