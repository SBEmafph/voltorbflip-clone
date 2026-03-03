#include <QtNetwork>

#include "Client.h"
#include "Config.h"

Client::Client(QObject *parent)
    : QObject{parent}
    , m_tcpSocket (new QTcpSocket(this))
{
    m_in.setDevice(m_tcpSocket);
    m_in.setVersion(QDataStream::Qt_6_5);
    connect(m_tcpSocket, &QTcpSocket::readyRead,
            this, &Client::slot_onReadyRead);

    m_out.setDevice(m_tcpSocket);
    m_out.setVersion(QDataStream::Qt_6_5);

    connect(this, &Client::sig_connected,
            this, &Client::slot_joinSelectedLobby);
}

void Client::m_loadConfig(bool fForce)
{
    bool settingChanged = false;
    QJsonObject config = VOF::loadOrCreateConfig();
    quint32 id = config["playerID"].toInt();
    quint16 token = config["sessionToken"].toInt();
    QString name = config["playerName"].toString();
    if(fForce || (id != m_dwID)){
        m_dwID = id;
        settingChanged = true;
    }

    if(fForce || (token != m_wToken)) {
        m_wToken = token;
        settingChanged = true;
    }

    if(fForce || QString::compare(m_szName, name, Qt::CaseInsensitive)){
        m_szName = name;
        settingChanged = true;
    }

    if(settingChanged)
        m_updateConfig(m_dwID, m_wToken, m_szName, fForce);
}

quint16 Client::m_packMove(VOF::Action action, quint8 x, quint8 y) {
    return static_cast<quint16>((action << 8) | (x << 4) | y);
    //click '0' on field (2,3) -> 0x0230
}

void Client::slot_onReadyRead()
{
    while (m_tcpSocket->bytesAvailable() > 0) {
        m_in.startTransaction();
        quint8 command;
        m_in >> command;

        LOG_OUT << "clientOnReadyRead command " << command << Qt::endl;

        switch (command){
        case VOF::Command::LoginRequest:
            if (!m_in.commitTransaction()) return;
            m_sendIdentification();
            break;
        case VOF::Command::NewLoginConfig:
            quint32 tempID;
            quint16 tempToken;
            m_in >> tempID;
            m_in >> tempToken;

            if (!m_in.commitTransaction()) {
                return;
            }
            LOG_OUT << "clientOnReadyRead Prev " << m_dwID << " : " << m_wToken << Qt::endl;
            m_dwID = tempID;
            m_wToken = tempToken;
            LOG_OUT << "clientOnReadyRead After " << m_dwID << " : " << m_wToken << Qt::endl;
            VOF::writeConfig(m_dwID, m_wToken, m_szName);
            emit sig_newLoginSaved();
            break;
        case VOF::Command::LoginSuccess:
            if (!m_in.commitTransaction()) return;
            emit sig_connected();
            break;
        default:
            if (!m_in.commitTransaction()) return;
            break;
        }
    }
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

void Client::slot_attach(QHostAddress ipAdressIn, quint16 portIn)
{
    if(!m_tcpSocket)
        m_setTcpSocket(new QTcpSocket(this));

    QAbstractSocket::SocketState currentState = m_tcpSocket->state();
    LOG_OUT << "Attempting attach. Current State: " << currentState << Qt::endl;

    if (currentState != QAbstractSocket::ConnectedState) {
        LOG_OUT << "Not Connected. M_attach..." << Qt::endl;
        m_tcpSocket->connectToHost(ipAdressIn, portIn);

        if (!m_tcpSocket->waitForConnected(3000)) {
            LOG_OUT << "Could not connect: " << m_tcpSocket->errorString() << Qt::endl;
            //startServer();
            emit sig_connected();
            return;
        }
    }
    emit sig_connected();
}

void Client::slot_detach()
{
    if (!m_tcpSocket) {
        LOG_OUT << "Detach aborted: m_tcpSocket is nullptr!" << Qt::endl;
        return;
    }
    QAbstractSocket::SocketState currentState = m_tcpSocket->state();
    LOG_OUT << "Attempting detach. Current State: " << currentState << Qt::endl;

    if (currentState == QAbstractSocket::ConnectedState) {
        LOG_OUT << "Sending Quit-Command before application closes..." << Qt::endl;
        if (m_tcpSocket->isWritable()) {
            m_out << VOF::Command::Quit;
            m_tcpSocket->flush();
            m_tcpSocket->waitForBytesWritten(1000);
        }
        m_tcpSocket->disconnectFromHost();
    }else if (currentState
        != QAbstractSocket::UnconnectedState) {
        LOG_OUT << "Socket not connected, but busy. Aborting..." << Qt::endl;
        m_tcpSocket->abort();
    } else {
        LOG_OUT << "Socket already disconnected." << Qt::endl;
    }
}

void Client::m_sendIdentification()
{
    LOG_OUT << "Connected. Sending Identification" << Qt::endl;
    //only read if values are empty
    if((m_dwID == 0) || (m_wToken == 0) || (m_szName.isEmpty()))
        m_loadConfig();

    quint8 command = VOF::Command::LoginRequest;
    m_out << command;
    m_out << m_dwID;
    m_out << m_wToken;
    m_tcpSocket->flush();
    emit sig_identificationSent();
}

void Client::slot_joinSelectedLobby()
{
    //for testing we will only use one lobby
    if(!m_fInLobby){
        m_out << VOF::Command::LobbyJoinRequest;
        m_out << m_dwID;
        m_out << m_wToken;
        m_out << m_selectedLobbyID;
        m_tcpSocket->flush();
        m_fInLobby = true;
    }
}

void Client::m_updateConfig(quint32 dwIDIn, quint16 wtokenIN, QString szNameIn, bool fForce)
{
    bool settingChanged = false;
    QJsonObject config = VOF::loadOrCreateConfig();
    quint32 id = config["playerID"].toInt();
    quint16 token = config["sessionToken"].toInt();
    QString name = config["playerName"].toString();

    if(fForce || (id == 0)){
        m_dwID = dwIDIn;
        settingChanged = true;
    }
    else{
        m_dwID = id;
    }

    if(fForce || (id == 0)){
        m_wToken = wtokenIN;
        settingChanged = true;
    }
    else{
        m_wToken = token;
    }

    if(fForce || name.isEmpty()){
        m_szName = szNameIn;
        settingChanged = true;
    }
    else{
        m_szName = name;
    }

    if(settingChanged){
        VOF::writeConfig(m_dwID, m_wToken, m_szName, fForce);
    }
}

void Client::slot_sendMatchAction(VOF::Action action, quint8 x, quint8 y)
{
    LOG_OUT << "action " << action << " x: " << x << " y: " << y << Qt::endl;
    quint16 packedMove = m_packMove(action, x, y);
    LOG_OUT << packedMove << Qt::endl;
    m_out << VOF::Command::PlayerMove;
    m_out << packedMove;
    m_tcpSocket->flush();
}
