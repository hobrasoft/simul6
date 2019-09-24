#ifndef _MESSAGE_DIALOG_H_
#define _MESSAGE_DIALOG_H_

#include <QDialog>
#include <QList>
#include "ui_messagedialog.h"
#include "message.h"

#define SHOWMESSAGE(x) MessageDialog::showMessage(x)

class WorkerMessage;

class Message_worker : public QObject {
    Q_OBJECT
  public:
    Message_worker(QObject *p) : QObject(p) { connect(this, SIGNAL(sigAddEvent(Message)), this, SLOT(slotAddEvent(Message)), Qt::QueuedConnection); }

    void addEvent(Message m) { emit sigAddEvent(m); }

  signals:
    void sigAddEvent(Message);

  private slots:
    void slotAddEvent(Message);

};


/**
 * @brief Dialogové okno pro zobrazení zpráv aplikace Fotomon
 *
 * Okno je přizpůsobené vzhledu aplikace Fotomon.
 * Jako vstup se používá objekt Message
 * Zobrazovaných zpráv může být několik, dá se mezi nimi listovat.
 */
class MessageDialog : public QDialog, Ui::MessageDialog {
    Q_OBJECT
  public:
    static  void setShowTime  (bool x);
    static  void setShowLabels(bool x);
    static  void showMessage(Message e);
    static  void showMessage(QString s);
    static  void showMessage(const WorkerMessage& message);

  public slots:
    void    slotAddEvent(Message e);

  private slots:
    void    slotPrevious();
    void    slotNext();
    void    slotClose();

  signals:
    void    sigAddEvent(Message e);

  private:
   ~MessageDialog();
    MessageDialog();
    void    showEvent(Message e);
    void    showEvent(int);

    QList<Message> m_events;
    int     m_index;
    bool    m_showTime;
    bool    m_showLabels;

    static  MessageDialog *mDialog;

};

#endif
