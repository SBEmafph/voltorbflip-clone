
#include <QtNetwork>
#include <QtCore>
#include <QTextStream>

#include "VoltOrbFlipServer.h"
#include "Constants.h"

QTextStream err(stderr);
QTextStream out(stdout);

VoltOrbFlipServer::VoltOrbFlipServer(QObject *parent)
    : m_pTcpServer (new QTcpServer(this) )
{
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &VoltOrbFlipServer::slot_attach);
    m_startServer(16000);
}

void VoltOrbFlipServer::slot_notifyClients()
{
    foreach (NetworkObserver* player, m_clients) {
        player->m_updateStates(m_tGamestate);
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

        for (NetworkObserver* obs : m_clients.values()) {
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
        err
            << "Unable to start the server: %s"
            << m_pTcpServer->errorString()
            << Qt::endl;
        return;
    }
    out << "The server is running on IP: " << ipAddress.toString() << Qt::endl;
    out << "Port: " << m_pTcpServer->serverPort() << Qt::endl;
}

void VoltOrbFlipServer::slot_attach()
{
    QTcpSocket* clientConnection = m_pTcpServer->nextPendingConnection();
    NetworkObserver* player = new NetworkObserver(clientConnection, this);

    player->m_requestIdentification();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);
    connect(player, &NetworkObserver::sig_identificationReceived,
            this, &VoltOrbFlipServer::slot_processHandshake);
    connect(player, &NetworkObserver::sig_newAccountRequested,
            this, &VoltOrbFlipServer::slot_provideNewLogin);
    connect(player, &NetworkObserver::sig_quit,
            this, &VoltOrbFlipServer::slot_detach);
}

void VoltOrbFlipServer::slot_assignLobby(NetworkObserver* pNWObs, quint8 lobbyIDin)
{
    if(m_lobby.count() < VOF::MAX_CLIENTS){
        quint8 slotID = m_findFirstFreeSlot();
        m_lobby.insert(slotID, pNWObs);
        pNWObs->m_setSlotId(lobbyIDin);
        out << "Welcome Player " << pNWObs->m_getId()
            << " in Lobby lobbyIDin" << lobbyIDin
            << " on lobbyslot " << slotID << Qt::endl;
    }
    else{
        out << "ERR: Lobby " << lobbyIDin
            << " Already Full" << Qt::endl;
    }
}

void VoltOrbFlipServer::slot_processHandshake(quint32 idIn, quint16 tokenIn)
{
    out << "INF: Handshake with " << idIn
        << " on " << tokenIn << Qt::endl;

    NetworkObserver* anon = qobject_cast<NetworkObserver*>(sender());
    connect(anon->m_getSocket(), &QAbstractSocket::disconnected,
            anon, &QObject::deleteLater);

    if(m_playerDatabase.contains(idIn)){ //known ID
        if(m_clients.contains(idIn)){ //currently playing
            NetworkObserver* existingPlayer = m_clients[idIn];
            if(existingPlayer->m_getToken() == tokenIn) //correct token -> recomnnect
            {
                out << "INF: Welcome back " << idIn << Qt::endl;
                existingPlayer->m_setSocket(anon->m_getSocket()); //implied connection issue
                anon->deleteLater();
                return;
            }
            else{ //incorrect token -> impostor among us
                err << "ERR: Wrong token for slot " << idIn << Qt::endl;
                anon->m_getSocket()->disconnectFromHost();
                return;
                //slot_provideNewLogin(anon);
            }
        }
        else{ //first login this server life -> refresh token
            slot_refreshToken(anon);
            return;
        }
    }
    else{ //new player
        //out << "ERR: ClientID not usable" << Qt::endl;
        slot_provideNewLogin(anon);
        return;
    }
}

void VoltOrbFlipServer::slot_provideNewLogin(NetworkObserver * pNetworkObserver)
{
    out << "INF: New Login Requested" << Qt::endl;
    quint32 dwID = m_getNextFreePlayerId();
    quint16 wToken = m_generateUniqueToken();
    //out << "serverProvideLogin " << dwID << " : " << wToken << Qt::endl;
    PlayerProfile player;
    player.id = dwID;
    player.token = wToken;
    m_playerDatabase.insert(dwID, player);
    m_clients.insert(dwID, pNetworkObserver);
    out << "INF: Welcome " << dwID << " to the server" << Qt::endl;
    pNetworkObserver->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_refreshToken(NetworkObserver *pNetworkObserver)
{
    quint32 dwID = pNetworkObserver->m_getId();
    quint16 wToken = m_generateUniqueToken();
    out << "serverProvideLogin " << dwID << " : " << wToken << Qt::endl;
    PlayerProfile player = m_playerDatabase[dwID];
    player.token = wToken;
    m_clients.insert(dwID, pNetworkObserver);
    out << "INF: Welcome " << dwID << " back to the server" << Qt::endl;
    pNetworkObserver->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_detach(NetworkObserver *obs)
{
    quint32 dwID = obs->m_getId();
    out << "INF: Goodbye " << dwID << Qt::endl;
    m_lobby.remove(dwID);
    m_clients.remove(dwID);
    obs->deleteLater();
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
