#include <QTextStream>

#include "NetworkObserver.h"
#include "GlobalDefines.h"

NWObs::NWObs(QTcpSocket* clientSocket, QObject* parent)
    :  m_pTcpSocket(clientSocket)
    ,  QObject(parent)
{
    m_dwID = 0;
    m_wToken = 0;
    m_in.setDevice(m_pTcpSocket);
    m_in.setVersion(QDataStream::Qt_6_5);
    connect(m_pTcpSocket, &QTcpSocket::readyRead,
            this, &NWObs::slot_onReadyRead);

    m_out.setDevice(m_pTcpSocket);
    m_out.setVersion(QDataStream::Qt_6_5);
}

void NWObs::updateFullState(const GameState &state)
{
    QByteArray data = m_prepareDataForPlayer(state);
}

void NWObs::onPlayerStatusChanged(quint32 dwPlayerId, bool fIsReady)
{

}

void NWObs::onTileRevealed(quint32 dwPlayerId, quint8 bTileIndex, quint8 bValue)
{

}


void NWObs::m_requestIdentification()
{
    //in case it isn't hardcoded: static_cast<quint8>(VOF::Command::LoginRequest);
    m_out << VOF::Command::LoginRequest;
    m_pTcpSocket->flush();
}

void NWObs::m_updateLogin(quint32 dwIDin, quint16 wTokenIn)
{
    LOG_OUT << "[NW] updateLogin " << dwIDin << " : " << wTokenIn << Qt::endl;
    m_setId(dwIDin);
    m_setToken(wTokenIn);
    m_out << VOF::Command::NewLoginConfig;
    m_out << dwIDin;
    m_out << wTokenIn;
    m_out << VOF::Command::LoginSuccess;
    m_pTcpSocket->flush();
}

void NWObs::m_proceedToMatch()
{
    return;
}

void NWObs::m_reconnect(QTcpSocket *SocketIn)
{
    m_pTcpSocket->abort();
    m_pTcpSocket = SocketIn;
    m_pTcpSocket->setParent(this);

    m_in.setDevice(m_pTcpSocket);
    m_out.setDevice(m_pTcpSocket);
    /*
     *     connect(m_socket, &QTcpSocket::readyRead,
            this, &NetworkObserver::m_onReadyRead);
     */
}

void NWObs::slot_onReadyRead()
{
    m_in.startTransaction();
    quint8 command;
    m_in >> command;
    quint32 id;
    quint16 token;

    if (!m_in.commitTransaction())
        return;

    switch (command){
    case VOF::Command::LoginRequest:

        m_in >> id;
        m_in >> token;
        LOG_OUT << "[NW] loginrequest " << id << " : " << token << Qt::endl;
        if(id == 0 || token == 0){
            emit sig_newAccountRequested(this);
        }
        else{
            m_setId(id);
            m_setToken(token);
            emit sig_identificationReceived(this, id, token);
        }
        break;
    case VOF::Command::LobbyJoinRequest:
        m_in >> id;
        m_in >> token;
        quint8 lobby;
        m_in >> lobby;
        LOG_OUT << "[NW] lobbyJoin " << id << " : " << token << Qt::endl;
        if((id == m_dwID) && (token == m_wToken))
            emit sig_lobbyRequest(this, lobby);
        else
            emit sig_identificationReceived(this, id, token);
        break;
    case VOF::Command::Quit:
        //out << "nwobs quit";
        emit sig_quit(this);
    default:
        break;
    }
}

void NWObs::slot_loginSuccessful()
{
    //m_out << VOF::Command::LoginSuccess;
    //m_pTcpSocket->flush();
}

QByteArray NWObs::m_prepareDataForPlayer(const GameState &)
{
    QByteArray a;
    return a;
}
