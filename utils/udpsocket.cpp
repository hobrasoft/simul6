/**
 * @file
 *
 * @author Petr Bravenec petr.bravenec@hobrasoft.cz
 */

#include "udpsocket.h"
#include "pdebug.h"


UdpSocket::UdpSocket(QObject *parent, const QHostAddress& ip, int port) : QObject(parent) {
    m_ip = ip;
    m_port = port;
    m_socket = new QUdpSocket(this);
    m_socket->bind(ip, port);
    connect(m_socket, SIGNAL(readyRead()),
            this,       SLOT(readPendingDatagrams()));
}


void UdpSocket::readPendingDatagrams() {
    while (m_socket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        m_socket->readDatagram(data.data(), data.size(), &sender, &senderPort);
        emit datagram(sender, data, senderPort);
        }
}


bool UdpSocket::send(const QHostAddress& ip, const QByteArray& data) {
    int bytessent = m_socket->writeDatagram(
            data,
            ip,
            m_port);

    if (bytessent < 0) {
        //PDEBUG << m_socket->error();
        return false;
        }

    m_socket->waitForBytesWritten(1000);

    return (bytessent > 0);
}


bool UdpSocket::send(const QHostAddress& ip, int port, const QByteArray& data) {
    if (port <= 0) {
        port = m_port;
        }
    int bytessent = m_socket->writeDatagram(
            data,
            ip,
            port);

    if (bytessent < 0) {
        //PDEBUG << m_socket->error();
        return false;
        }

    m_socket->waitForBytesWritten(1000);

    return (bytessent > 0);
}

