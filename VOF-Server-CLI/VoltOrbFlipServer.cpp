
#include <QtNetwork>
#include <QtCore>
#include <QTextStream>

#include "VoltOrbFlipServer.h"
#include "Constants.h"

QTextStream err(stderr);
QTextStream out(stdout);

VoltOrbFlipServer::VoltOrbFlipServer(QObject *parent)
    : m_tcpServer (new QTcpServer(this) )
{
    connect(m_tcpServer, &QTcpServer::newConnection, this, &VoltOrbFlipServer::m_attach);
    m_startServer(16000);
}

void VoltOrbFlipServer::m_notifyClients()
{
    foreach (NetworkObserver* player, m_clients) {
        player->m_updateStates(m_gamestate);
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

    if (!m_tcpServer->listen(ipAddress, port)) {
        err
            << "Unable to start the server: %s"
            << m_tcpServer->errorString()
            << Qt::endl;
        return;
    }
    out << "The server is running on IP: " << ipAddress.toString() << Qt::endl;
    out << "Port: " << m_tcpServer->serverPort() << Qt::endl;
}

void VoltOrbFlipServer::m_attach()
{
    QTcpSocket* clientConnection = m_tcpServer->nextPendingConnection();
    NetworkObserver* player = new NetworkObserver(clientConnection, this);

    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);
    connect(player, &NetworkObserver::m_identify,
            this, &VoltOrbFlipServer::m_processHandshake);
}

void VoltOrbFlipServer::m_processHandshake(quint32 idIn, quint16 tokenIn)
{
    NetworkObserver* anon = qobject_cast<NetworkObserver*>(sender());
    connect(anon->m_getSocket(), &QAbstractSocket::disconnected,
            anon, &QObject::deleteLater);

    if(m_clients.contains(idIn)){
        NetworkObserver* existingPlayer = m_clients[idIn];
        if(existingPlayer->m_getToken() == tokenIn)
        {
            existingPlayer->m_setSocket(anon->m_getSocket());
            anon->deleteLater();
        }
        else{
            err << "ERR: Wrong token for slot" << idIn << Qt::endl;
            anon->m_getSocket()->disconnectFromHost();
        }
    }
    else{
        if(m_clients.count() < VOF::MAX_CLIENTS){
            quint8 slotID = m_findFirstFreeSlot();
            m_clients.insert(slotID, anon);
            out << "Welcome Player " << idIn
                << "on lobbyslot " << slotID << Qt::endl;
        }
        else{
            out << "ERR: Already Full" << Qt::endl;
        }
    }

}

void VoltOrbFlipServer::m_detach(NetworkObserver *obs)
{

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
