#include <QtNetwork>

#include "client.h"
#include <QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
    , m_tcpSocket (new QTcpSocket)
{

}

void Client::m_attach()
{
    qDebug() << "m_attach()";
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(QHostAddress::LocalHost, 16000);
}
