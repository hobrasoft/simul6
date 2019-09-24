#include "messagedialog.h"
#include "pdebug.h"
#include <QDebug>
#include <QThread>
#include <QTimer>


MessageDialog *MessageDialog::mDialog = nullptr;

void Message_worker::slotAddEvent(Message m) {
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        }
    MessageDialog::showMessage(m);
    deleteLater();
}


MessageDialog::~MessageDialog() {
}


void MessageDialog::showMessage(QString s) {
    showMessage(Message(s));
}


void MessageDialog::showMessage(Message e) {
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        Message_worker *worker = new Message_worker(0);
        worker->moveToThread(QCoreApplication::instance()->thread());
        worker->addEvent(e);
        return;
        }

    if (mDialog == nullptr) {
        mDialog = new MessageDialog();
        }

    mDialog->slotAddEvent(e);
}

/**
 * Konstruktoru se předává jako parametr událost, která se má zobrazit. Pro
 * zobrazení stačí zavolat exec() nebo show() - záleží, jak se má aplikace chovat.
 */
MessageDialog::MessageDialog()
        : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
    setupUi(this);
    m_index = 0;
    m_showTime = true;
    m_showLabels = true;
    connect(f_previous, SIGNAL(clicked()), this, SLOT(slotPrevious()));
    connect(f_next,     SIGNAL(clicked()), this, SLOT(slotNext()));
    connect(f_cancel,   SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(this,       SIGNAL(sigAddEvent(Message)), this, SLOT(slotAddEvent(Message)));
    setModal(true);
    hide();
}


/**
 * @brief Přepínač pro zobrazování času
 *
 * Některé zprávy nejsou časově kritické, informace o času je u nich
 * zbytečná (například kontrola nové verze).
 */
void MessageDialog::setShowTime  (bool x) { 
    Q_ASSERT(mDialog);
    mDialog->m_showTime = x;
    mDialog->showEvent( mDialog->m_events.size()-1 );
} 


/**
 * @brief Přepínač pro zobrazování popisků
 *
 * U zpráv, které obsahují pouze "Hlášení" je někdy zbytečné zobrazovat
 * popisky. Předáním false se dají popisky vypnout
 */
void MessageDialog::setShowLabels(bool x) { 
    Q_ASSERT(mDialog);
    mDialog->m_showLabels = x;
    mDialog->showEvent( mDialog->m_events.size()-1 );
}


/**
 * @brief Přidá k seznamu novou zprávu a nastaví ji jako zobrazovanou
 */
void MessageDialog::slotAddEvent(Message e) {

    // Projít staré události, aby nedocházelo k duplicitám
    for (int i=0; i<m_events.size(); i++) {
        Message x = m_events.at(i);
        if (
            x.time()        == e.time() &&
            x.message()     == e.message() &&
            x.description() == e.description()) {
            // duplicita, pouze zobrazit a nepřidávat
            showEvent(i);
            goto konec;
            }
        }
    m_events.append(e);
    showEvent( m_events.size()-1 );

  konec:
//  SPLASH->hide();
    show();
//  showNormal();
//  raise();
//  activateWindow();
//  setWindowState(Qt::WindowActive);

}


void MessageDialog::showEvent(int i) {
    m_index = i;
    if (m_index < 0 || m_index >= m_events.size()) {
        return;
        }
    f_previous->setEnabled( (m_index>0) );
    f_next    ->setEnabled( (m_index<m_events.size()-1) );
    showEvent( m_events.at(m_index) );
}


void MessageDialog::showEvent(Message e) {
    f_number->setText(QString("%1/%2").arg(m_index+1).arg(m_events.size()));
    f_description -> setText(e.description());

    if (e.time().isValid() && m_showTime) {
        f_time->show();
        fl_time->setVisible(m_showLabels);
        f_time-> setText(e.time().toString(tr("yyyy-MM-dd hh:mm:ss")));
      } else {
        f_time->hide();
        fl_time->hide();
        }

    if (!e.message().isEmpty()) {
        fl_message->setVisible(m_showLabels);
        f_message-> setText(e.message());
      } else {
        f_message-> setText(e.description());
        }

}


void MessageDialog::slotPrevious() {
    showEvent(--m_index);
}


void MessageDialog::slotNext() {
    showEvent(++m_index);
}


void MessageDialog::slotClose() {
    mDialog = nullptr;
    deleteLater();
}

