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

    connect(this, &NWObs::sig_newLoginSaved,
            this, &NWObs::slot_loginSuccessful);

    m_out.setDevice(m_pTcpSocket);
    m_out.setVersion(QDataStream::Qt_6_5);
}

void NWObs::updateFullState(const GameState &state)
{
    m_out << VOF::Command::GameStateUpdate;
    //LOG_OUT << "[NW] Updating full state " << Qt::endl;
    quint8 playerCount = state.tPlayerList.size();
    m_out << playerCount;

    for (auto it = state.tPlayerList.begin(); it != state.tPlayerList.end(); ++it)
    {
        quint8 slotId = it.key();
        const PlayerSessionState& player = it.value();

        m_out << slotId;
        m_out << player.tIdentity.name;
        m_out << player.fIsReady;
        m_out << player.bCurrentScore;
        m_out << player.bTotalScore;
        m_out << player.bLevel;

        LOG_OUT << player.bBoard[0] << Qt::endl;

        // Nur revealed Tiles übertragen
        for (int i = 0; i < 25; ++i)
        {
            if (slotId == m_bLobbySlotID){
                m_out << static_cast<quint8>(i);
                m_out << static_cast<quint8>(player.bBoard[i] + player.fRevealed[i] * 8);
            }
            else{
                if(player.fRevealed[i]){
                    m_out << static_cast<quint8>(i); // Position
                    m_out << player.bBoard[i];       // Wert
                }
            }
        }
        m_out << static_cast<quint8>(255);
    }
}

void NWObs::slot_PlayerReadyState(quint32 dwPlayerId, bool fIsReady)
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

void NWObs::m_sendStartMatch()
{
    m_out << VOF::Command::StartMatch;
}

void NWObs::m_updateLogin(quint32 dwIDin, quint16 wTokenIn)
{
    m_waitNewLoginAck = true;

    LOG_OUT << "[NW] updateLogin " << dwIDin << " : " << wTokenIn << Qt::endl;
    m_setId(dwIDin);
    m_setToken(wTokenIn);
    m_out << VOF::Command::NewLoginConfig;
    m_out << dwIDin;
    m_out << wTokenIn;
    m_pTcpSocket->flush();
}

void NWObs::m_assignSlotID(quint8 bSlotID)
{
    m_out << VOF::Command::LobbyJoinRequest;
    m_out << bSlotID;
    m_setSlotID(bSlotID);
}

void NWObs::sendLobbySlotInfo(QString szName, quint8 bSlotID, bool fIsReady)
{
    LOG_OUT << "[NW] updateLobby " << szName << " : " << bSlotID << " : " << fIsReady << Qt::endl;
    m_out << VOF::Command::LobbyUpdate;
    m_out << szName;
    m_out << bSlotID;
    m_out << fIsReady;
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

    //LOG_OUT << "command " << command << Qt::endl;
    //LOG_OUT << "Bytes available: " << m_pTcpSocket->bytesAvailable() << Qt::endl;

    if(m_waitNewLoginAck)
    {
        if(command == VOF::Command::LoginRequest){
            if (m_in.commitTransaction()) {
                m_waitNewLoginAck = false;
                quint32 id;
                quint16 token;
                QString name;
                m_in >> id >> token >> name;  //not used, but needs to be removed out of the message queue
                emit sig_newLoginSaved();
                LOG_OUT << "[NW] new login saved; login successfull" << Qt::endl;
            }
            return;
        }
        m_in.rollbackTransaction();
        return;
    }

    quint32 dwID;
    quint16 wToken;
    quint8 bLobbyID;
    quint16 wPackedMove;
    QString szName;
    quint8 bSlotID;
    bool fIsReady;

    switch (command) {
    case VOF::Command::LoginRequest:
        m_in >> dwID >> wToken >> szName;
        break;
    case VOF::Command::LobbyJoinRequest:
        m_in >> dwID >> wToken >> bLobbyID;
        //LOG_OUT << "lobbyjoinrequest " << " id " << dwID  << " token " << wToken << " lobby " << bLobbyID << Qt::endl;
        break;
    case VOF::Command::LobbyUpdate:
        m_in >> bLobbyID >> bSlotID >> fIsReady;
        //LOG_OUT << "ready " << fIsReady << " lobby " << bLobbyID << " slot " << bSlotID << Qt::endl;
        break;
    case VOF::Command::PlayerMove:
        m_in >> wPackedMove;
        break;
    case VOF::Command::StartMatch:
        break;
    case VOF::Command::Quit:
        break;
    default:
        m_in.rollbackTransaction();
        return;
    }

    if (!m_in.commitTransaction())
        return;

    switch (command){
    case VOF::Command::LoginRequest:
        LOG_OUT << "[NW] loginrequest " << dwID << " : " << wToken << " : " << szName << Qt::endl;
        m_setName(szName);
        if(dwID == 0 || wToken == 0){
            emit sig_newAccountRequested(this);
        }
        else{
            m_setId(dwID);
            m_setToken(wToken);
            //LOG_OUT << m_szName << m_getName() << Qt::endl;
            emit sig_identificationReceived(this, dwID, wToken);
        }
        break;
    case VOF::Command::LobbyJoinRequest:
        LOG_OUT << "[NW] lobbyJoin " << dwID << " : " << wToken << Qt::endl;
        if((dwID == m_dwID) && (wToken == m_wToken))
            emit sig_lobbyRequest(this, bLobbyID);
        else
            emit sig_identificationReceived(this, dwID, wToken);
        break;

    case VOF::Command::LobbyUpdate:
        //TODO: in gamestate eintragen
        LOG_OUT << "ready " << fIsReady
                << " lobby " << bLobbyID
                << " slot " << bSlotID << Qt::endl;
        emit sig_readyStateChanged(bLobbyID, bSlotID, fIsReady);
        //TODO: update senden
        break;
    case VOF::Command::PlayerMove:
        LOG_OUT << "[NW] PlayerMove " << wPackedMove << Qt::endl;
        emit sig_playerMove(this, wPackedMove);
        break;
    case VOF::Command::StartMatch:
        LOG_OUT << "[NW] StartMatch requested" << Qt::endl;
        emit sig_startMatchRequested(this);
        break;
    case VOF::Command::Quit:
        //out << "nwobs quit";
        emit sig_quit(this);
        break;
    default:
        break;
    }
}

void NWObs::slot_loginSuccessful()
{
    m_out << VOF::Command::LoginSuccess;
    //m_pTcpSocket->flush();
}
