
#include <QtNetwork>
#include <QtCore>
#include <QTextStream>

#include "voltOrbFlipServer.h"

QTextStream err(stderr);
QTextStream out(stdout);

VoltOrbFlipServer::VoltOrbFlipServer(QObject *parent)
    : m_tcpServer (new QTcpServer(this) )
{
    connect(m_tcpServer, &QTcpServer::newConnection, this, &VoltOrbFlipServer::attach);
    startServer(16000);
}

void VoltOrbFlipServer::notifyClients()
{
    foreach (NetworkObserver* player, m_clients) {
        player->updateStates(m_gamestate);
    }
}

quint8 VoltOrbFlipServer::m_findFirstFreeSlot()
{
    quint8 key = 0;
    while(m_clients.contains(key))
    {
        key++;
    }
    return key;
}

void VoltOrbFlipServer::startServer(quint16 port)
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

    m_tcpServer = new QTcpServer(this);
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

void VoltOrbFlipServer::attach()
{
    quint8 slotID = 8;
    if(m_clients.count() < MAX_CLIENTS){
        slotID = m_findFirstFreeSlot();
        QTcpSocket* clientConnection = m_tcpServer->nextPendingConnection();
        NetworkObserver* player = new NetworkObserver(clientConnection);
        m_clients.insert(slotID, player);
        connect(clientConnection, &QAbstractSocket::disconnected,
                clientConnection, &QObject::deleteLater);
        out << "Welcome Player" << slotID << Qt::endl;
    }
    else{
        out << "ERR: Already Full" << Qt::endl;
    }
}

void VoltOrbFlipServer::detach(NetworkObserver *obs)
{

}

void VoltOrbFlipServer::processInput(int playerId, std::string Input)
{

}

bool VoltOrbFlipServer::verifyInput(std::string input)
{
    return true;
}

void VoltOrbFlipServer::applyMove(std::string input)
{

}
