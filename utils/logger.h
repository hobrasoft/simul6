/**
 * @file
 * 
 * @author Petr Bravenec <petr.bravenec@hobrasoft.cz>
 */
#ifndef _Logger_H_
#define _Logger_H_

#include <QObject>
#include <QMessageLogContext>

/**
 * @brief Třída filtruje a loguje do souboru všechna hlášení z aplikace vzniklé pomocí qDebug()
 */
class Logger : public QObject {
    Q_OBJECT
  public:
    static Logger *instance(QObject *parent = nullptr);
    static void messageOutput(const QString&);


  private:
    static Logger *m_instance;
    Logger(QObject *);

    static void messageOutput(QtMsgType type, const QMessageLogContext&, const QString& );

    static bool     m_log_all;          ///< Pokud je true, loguje se vše, kromě exclude
    static QString  m_log_exclude;      ///< Výraz se neloguje
    static QString  m_log_include;      ///< Výraz se loguje, pokud ovšem není v exclude
    static QString  m_log_file;         ///< Cesta k souboru s logem

};

#endif

