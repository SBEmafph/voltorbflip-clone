#include <QtNetwork>

#include "Client.h"
#include "Config.h"
#include "Constants.h"

#include <QDebug>

Client::Client(QObject *parent)
    : QObject{parent}
    , m_tcpSocket (new QTcpSocket(this))
{
    m_in.setDevice(m_tcpSocket);
    m_in.setVersion(QDataStream::Qt_6_5);
}

void Client::m_readLobbyState() //notworking
{
    m_in.startTransaction();
    quint8 ID;

    if (!m_in.commitTransaction())
        return;
}

bool Client::m_checkStartLobby() //notworking
{
    m_in.startTransaction();
    bool everyoneReady = false;
    if (!m_in.commitTransaction())
        return false;
    m_in >> everyoneReady;
    return everyoneReady;
}

void Client::m_attach()
{
    qDebug() << "m_attach()";
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost(QHostAddress::LocalHost, 16000);
}

void Client::m_identify()
{
    QJsonObject config = VOF::loadOrCreateConfig();

    quint8 command = VOF::LoginRequest;
    quint32 id = config["playerID"].toInt();
    quint16 token = config["sessionToken"].toInt();

    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << command;
    out << id;
    out << token;

    m_tcpSocket->write(packet);
}
