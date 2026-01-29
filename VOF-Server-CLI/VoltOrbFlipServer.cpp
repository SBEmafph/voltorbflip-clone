
#include <QtNetwork>
#include <QtCore>


#include "VoltOrbFlipServer.h"



VoltOrbFlipServer::VoltOrbFlipServer(QObject *parent)
    : m_pTcpServer (new QTcpServer(this) )
{
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &VoltOrbFlipServer::slot_attach);
    m_startServer(16000);
}

void VoltOrbFlipServer::slot_updateClientsGameState()
{
    foreach (NWObs* player, m_clients) {
        player->updateFullState(m_tGamestate);
    }
}

void VoltOrbFlipServer::slot_onLobbyStatusUpdate(quint32 dwPlayerId, bool fIsReady)
{
    foreach (NWObs* player, m_clients) {
        player->onPlayerStatusChanged(dwPlayerId, fIsReady);
    }
}

quint8 VoltOrbFlipServer::m_findFirstFreeSlot()
{
    quint8 slotId = 0;
    while(m_clients.contains(slotId))
    {
        slotId++;
    }
    return slotId;
}

quint32 VoltOrbFlipServer::m_getNextFreePlayerId()
{
    if (m_playerDatabase.isEmpty()) return 1;
    return m_playerDatabase.lastKey() + 1;
}

quint16 VoltOrbFlipServer::m_generateUniqueToken()
{
    quint16 newToken = 0;
    bool unique = false;
    while (!unique) {
        newToken = static_cast<quint16>(QRandomGenerator::global()->bounded(1, 65536));
        unique = true;

        for (NWObs* obs : m_clients.values()) {
            if (obs->m_getToken() == newToken) {
                unique = false;
                break;
            }
        }
    }
    return newToken;
}

void VoltOrbFlipServer::m_startServer(quint16 port)
{
    QHostAddress ipAddress;
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // for (const QHostAddress &entry : ipAddressesList) {
    //     if (entry != QHostAddress::LocalHost && entry.toIPv4Address()) {
    //         ipAddress = entry;
    //         break;
    //     }
    // }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isNull()){
        ipAddress = QHostAddress::LocalHost;
    }

    if (!m_pTcpServer->listen(ipAddress, port)) {
        LOG_ERR
            << "Unable to start the server: %s"
            << m_pTcpServer->errorString()
            << Qt::endl;
        return;
    }
    LOG_OUT << "[SV] The server is running on IP: " << ipAddress.toString() << Qt::endl;
    LOG_OUT << "[SV] Port: " << m_pTcpServer->serverPort() << Qt::endl;
}

void VoltOrbFlipServer::slot_attach()
{
    QTcpSocket* clientConnection = m_pTcpServer->nextPendingConnection();
    NWObs* player = new NWObs(clientConnection, this);

    player->m_requestIdentification();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);
    connect(player, &NWObs::sig_identificationReceived,
            this, &VoltOrbFlipServer::slot_processHandshake);
    connect(player, &NWObs::sig_newAccountRequested,
            this, &VoltOrbFlipServer::slot_provideNewLogin);
    connect(player, &NWObs::sig_lobbyRequest,
            this, &VoltOrbFlipServer::slot_assignLobby);
    connect(player, &NWObs::sig_quit,
            this, &VoltOrbFlipServer::slot_detach);

    connect(this, &VoltOrbFlipServer::sig_loginSuccessful,
            player, &NWObs::slot_loginSuccessful);

}

void VoltOrbFlipServer::slot_assignLobby(NWObs* pNWObs, quint8 lobbyIDin)
{
    if(m_lobby.count() < VOF::MAX_CLIENTS){
        quint8 slotID = m_findFirstFreeSlot();
        m_lobby.insert(slotID, pNWObs);
        pNWObs->m_setSlotId(lobbyIDin);
        LOG_OUT << "[SV] Welcome Player " << pNWObs->m_getId()
            << " in Lobby " << lobbyIDin
            << " on Lobby Slot " << slotID << Qt::endl;
        emit sig_lobbyUpdate(pNWObs, lobbyIDin);
    }
    else{
        LOG_OUT << "[SV] ERR: Lobby " << lobbyIDin
            << " Already Full" << Qt::endl;
    }
}

void VoltOrbFlipServer::slot_processHandshake(NWObs* pNWObs, quint32 idIn, quint16 tokenIn)
{
    LOG_OUT << "[SV] INF: Handshake with " << idIn
        << " on " << tokenIn << Qt::endl;

    //NWObs* pNWObs = qobject_cast<NWObs*>(sender());
    //connect(pNWObs->m_getSocket(), &QAbstractSocket::disconnected,
    //        pNWObs, &QObject::deleteLater);

    if(m_playerDatabase.contains(idIn)){ //known ID
        LOG_OUT << "[SV] known ID : " << idIn << Qt::endl;
        if(!m_clients.contains(idIn)){ //currently not connected
            PlayerProfile existingPlayer = m_playerDatabase[idIn];
            if(existingPlayer.token == tokenIn) //correct token -> recomnnect
            {
                LOG_OUT << "[SV] INF: Welcome back " << idIn << Qt::endl;
                emit sig_loginSuccessful();
                return;
            }
            else{ //first login this server life -> refresh token
                LOG_ERR << "[SV] ERR: Wrong token for slot " << idIn << Qt::endl;
                slot_refreshToken(pNWObs);
                return;
            }
            /* NWObs* existingPlayer = m_clients[idIn];
            // if(existingPlayer->m_getToken() == tokenIn) //correct token -> recomnnect
            // {
            //     out << "INF: Welcome back " << idIn << Qt::endl;
            //     existingPlayer->m_setSocket(pNWObs->m_getSocket()); //implied connection issue
            //     pNWObs->deleteLater();
            //     return;
            // }
            // else{ //incorrect token -> impostor among us
            //     err << "ERR: Wrong token for slot " << idIn << Qt::endl;
            //     pNWObs->m_getSocket()->disconnectFromHost();
            //     return;
            //     //slot_provideNewLogin(anon);
            */
        }//id already in use LOG_OUT << " [SV] id already connected :" << idIn << Qt::endl;
        else{ //first login this server life -> refresh token
            LOG_ERR << "ERR Handshake requested while connected" << Qt::endl;
            slot_refreshToken(pNWObs);
            return;
        }
    }
    else{ //new player
        //LOG_OUT << "ERR: ClientID not usable" << Qt::endl;
        LOG_OUT << "[SV] unknownID : " << idIn << Qt::endl;
        slot_provideNewLogin(pNWObs);
        return;
    }
}

void VoltOrbFlipServer::slot_provideNewLogin(NWObs * pNWObs)
{
    LOG_OUT << "[SV] INF: New Login Requested" << Qt::endl;
    quint32 dwID = m_getNextFreePlayerId();
    quint16 wToken = m_generateUniqueToken();
    LOG_OUT << "[SV] serverProvideLogin " << dwID << " : " << wToken << Qt::endl;
    PlayerProfile profile;
    profile.identity.id = dwID;
    profile.token = wToken;
    m_playerDatabase.insert(dwID, profile);
    m_clients.insert(dwID, pNWObs);
    LOG_OUT << "[SV] INF: Welcome " << dwID << " to the server" << Qt::endl;
    pNWObs->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_refreshToken(NWObs *pNWObs)
{
    quint32 dwID = pNWObs->m_getId();
    quint16 wToken = m_generateUniqueToken();
    LOG_OUT << "[SV] serverRefreshToken " << dwID << " : " << wToken << Qt::endl;
    PlayerProfile profile = m_playerDatabase[dwID];
    profile.token = wToken;
    m_clients.insert(dwID, pNWObs);
    LOG_OUT << "[SV] INF: Welcome " << dwID << " back to the server" << Qt::endl;
    pNWObs->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_detach(NWObs *pNWObs)
{
    quint32 dwID = pNWObs->m_getId();
    LOG_OUT << "[SV] INF: Goodbye " << dwID << Qt::endl << Qt::endl;
    m_lobby.remove(dwID);
    m_clients.remove(dwID);
    pNWObs->deleteLater();
}

void VoltOrbFlipServer::m_processInput(int playerId, std::string Input)
{

}

bool VoltOrbFlipServer::m_verifyInput(std::string input)
{
    return true;
}

void VoltOrbFlipServer::m_applyMove(std::string input)
{

}
