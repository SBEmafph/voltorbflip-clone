#include <QTextStream>
#include <QDebug>

#include "NetworkObserver.h"
#include "Constants.h"

NetworkObserver::NetworkObserver(QTcpSocket* clientSocket, QObject* parent)
    :  m_pTcpSocket(clientSocket)
    ,  QObject(parent)
{
    m_dwID = 0;
    m_wToken = 0;
    m_in.setDevice(m_pTcpSocket);
    m_in.setVersion(QDataStream::Qt_6_5);
    connect(m_pTcpSocket, &QTcpSocket::readyRead,
            this, &NetworkObserver::slot_onReadyRead);

    m_out.setDevice(m_pTcpSocket);
    m_out.setVersion(QDataStream::Qt_6_5);
}

void NetworkObserver::m_updateStates(const GameState &state)
{
    QByteArray data = m_prepareDataForPlayer(state);
}

void NetworkObserver::m_requestIdentification()
{
    //in case it isn't hardcoded: static_cast<quint8>(VOF::Command::LoginRequest);
    m_out << VOF::Command::LoginRequest;
    m_pTcpSocket->flush();
}

void NetworkObserver::m_updateLogin(quint32 dwIDin, quint16 wTokenIn)
{
    //qDebug() << "NWObs updateLogin " << dwIDin << " : " << wTokenIn << Qt::endl;
    m_setId(dwIDin);
    m_setToken(wTokenIn);
    m_out << VOF::Command::NewLoginConfig;
    m_out << dwIDin;
    m_out << wTokenIn;
    m_pTcpSocket->flush();
}

void NetworkObserver::m_proceedToMatch()
{
    return;
}

void NetworkObserver::m_reconnect(QTcpSocket *SocketIn)
{
    m_pTcpSocket->abort();
    m_pTcpSocket = SocketIn;
    m_pTcpSocket->setParent(this);

    m_in.setDevice(m_pTcpSocket);
    m_out.setDevice(m_pTcpSocket);
}

void NetworkObserver::slot_onReadyRead()
{
    m_in.startTransaction();
    quint8 command;
    m_in >> command;

    if (!m_in.commitTransaction())
        return;

    switch (command){
    case VOF::Command::LoginRequest:
        quint32 id;
        quint16 token;
        m_in >> id;
        m_in >> token;
        if(id == 0 || token == 0){
            emit sig_newAccountRequested(this);
        }
        else{
            emit sig_identificationReceived(id, token);
        }
        break;
    case VOF::Command::Quit:
        //qDebug() << "nwobs quit";
        emit sig_quit(this);
    default:
        break;
    }
}

QByteArray NetworkObserver::m_prepareDataForPlayer(const GameState &)
{
    QByteArray a;
    return a;
}
