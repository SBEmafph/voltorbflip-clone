#include <QTextStream>
#include <QDebug>

#include "NetworkObserver.h"
#include "Constants.h"

NetworkObserver::NetworkObserver(QTcpSocket* clientSocket, QObject* parent)
    :  m_socket(clientSocket)
    ,  QObject(parent)
{
    m_in.setDevice(m_socket);
    m_in.setVersion(QDataStream::Qt_6_5);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkObserver::m_onReadyRead);

    m_out.setDevice(m_socket);
    m_out.setVersion(QDataStream::Qt_6_5);

    connect(m_socket, &QTcpSocket::readyRead,
            this, &NetworkObserver::m_onReadyRead);
}

void NetworkObserver::m_updateStates(const GameState &state)
{
    QByteArray data = m_prepareDataForPlayer(state);
}

void NetworkObserver::m_proceedToMatch()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << "amogus";
    m_socket->write(data);
}

void NetworkObserver::m_reconnect(QTcpSocket *SocketIn)
{
    m_socket->abort();
    m_socket = SocketIn;
    m_socket->setParent(this);

    m_in.setDevice(m_socket);
    m_out.setDevice(m_socket);
}

void NetworkObserver::m_onReadyRead()
{
    m_in.startTransaction();
    quint8 command;
    m_in >> command;

    switch (command) //command
    {
    case VOF::Command::LoginRequest:
        quint32 id;
        quint16 token;
        m_in >> id;
        m_in >> token;

        if (!m_in.commitTransaction())
            return;

        qDebug() << "Emit signal" << Qt::endl;
        emit m_identify(id, token);
    default:
        break;
    }
}

QByteArray NetworkObserver::m_prepareDataForPlayer(const GameState &)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << "";
    return block;
}
