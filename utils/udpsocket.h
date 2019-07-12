/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#ifndef _UdpSocket_H_
#define _UdpSocket_H_

#include <QObject>
#include <QUdpSocket>
#include <QByteArray>
#include <QHostAddress>


/**
 * @brief Poslouchá na UDP a signálem předává přijatý paket dál
 */
class UdpSocket : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Konstruktor volaný z metody medibusSocket(QObject*, const QHostAddress&, int);
     *
     * @param parent - rodičovský objekt
     * @param ip - IP adresa, na kterou se má listener připojit, obvykle QHostAddress::Any
     * @param port - port, na kterém se má poslouchat, obvykle 6668
     *
     */
    UdpSocket(QObject *parent, const QHostAddress&, int port);

    /**
     * @brief Vrací číslo portu
     */
    int port() const { return m_port; }

    /**
     * @brief Vrací ip adresu
     */
    QHostAddress ip() const { return m_ip; }

    QUdpSocket  *socket() const { return m_socket; }

  signals:
    /**
     * @brief Předá jeden přijatý datagram
     * 
     * Předává se pouze reference - nedá se použít mezi vlákny
     */
    void    datagram(const QHostAddress& sender, const QByteArray& datagram, int port);

  public slots:

    /**
     * @brief Posílá jeden paket UDP protokolem 
     *
     * @param target - Cílová IP adresa
     * @param datagram - data pro odeslání
     */
    bool send(const QHostAddress& target, const QByteArray& datagram);

    /**
     * @brief Posílá jeden paket UDP protokolem 
     *
     * @param target - Cílová IP adresa
     * @param targetport - Cílový port
     * @param datagram - data pro odeslání
     */
    bool send(const QHostAddress& target, int targetport, const QByteArray& datagram);


    #ifndef DOXYGEN_SHOULD_SKIP_THIS
  private slots:
    void    readPendingDatagrams();

  private:
    QUdpSocket  *m_socket;                      ///< UDP Socket pro příjem datagramů
    QHostAddress m_ip;
    int          m_port;
    #endif

};

#endif
