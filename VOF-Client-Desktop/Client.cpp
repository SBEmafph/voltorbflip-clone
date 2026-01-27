#include <QtNetwork>

#include "Client.h"
#include "Config.h"


#include <QDebug>

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

    connect(this, &Client::sig_matchAction,
            this, &Client::slot_sendMatchAction);
}

void Client::m_loadConfig(bool force)
{
    bool settingChanged = false;
    QJsonObject config = VOF::loadOrCreateConfig();
    quint32 id = config["playerID"].toInt();
    quint16 token = config["sessionToken"].toInt();
    QString name = config["playerName"].toString();
    if(force || (id != m_dwID)) m_dwID = id;
    if(force || (token != m_wToken)) m_wToken = token;
    if(force || QString::compare(m_szName, name, Qt::CaseInsensitive))
        m_szName = name;

    if(settingChanged)
        m_updateConfig();
}

quint16 Client::m_packMove(VOF::Action action, quint8 x, quint8 y) {
    return static_cast<quint16>(action | (x << 4) | (y << 8));
    //click '0' on field (2,3) -> 0x0230
}

void Client::slot_onReadyRead()
{
    m_in.startTransaction();
    quint8 command;
    m_in >> command;

    if (!m_in.commitTransaction()) {
        return;
    }

    //qDebug() << "clientOnReadyRead command " << command << Qt::endl;

    switch (command){
    case VOF::Command::LoginRequest:
        m_sendIdentification();
        emit sig_identificationSent();
        break;
    case VOF::Command::NewLoginConfig:
        //qDebug() << "clientOnReadyRead Prev " << m_dwID << " : " << m_wToken << Qt::endl;
        m_in >> m_dwID;
        m_in >> m_wToken;
        //qDebug() << "clientOnReadyRead After " << m_dwID << " : " << m_wToken << Qt::endl;
        VOF::writeConfig(m_dwID, m_wToken, m_szName);
        emit sig_newLoginSaved();
        break;
    default:
        break;
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
        m_setTcpSocket(new QTcpSocket);

    m_tcpSocket->connectToHost(ipAdressIn, portIn);

    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Not Connected. M_attach...";

        if (!m_tcpSocket->waitForConnected(3000)) {
            qDebug() << "Could not connect:" << m_tcpSocket->errorString();
            //startServer();
            return;
        }
    }
}

void Client::slot_detach()
{
    if (m_tcpSocket &&
        m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Sending Quit-Command before application closes...";
        m_out << VOF::Command::Quit;
        m_tcpSocket->flush();
        m_tcpSocket->waitForBytesWritten(1000);
        m_tcpSocket->disconnectFromHost();
    }
}

void Client::m_sendIdentification()
{
    qDebug() << "Connected. Sending Identification";
    //only read if values are empty
    if((m_dwID == 0) || (m_wToken == 0) || (m_szName.isEmpty()))
        m_loadConfig();

    quint8 command = VOF::Command::LoginRequest;
    m_out << command;
    m_out << m_dwID;
    m_out << m_wToken;
    m_tcpSocket->flush();
}

void Client::m_updateConfig(bool force)
{
    VOF::writeConfig(m_dwID, m_wToken, m_szName, force);
}

void Client::slot_sendMatchAction(VOF::Action action, quint8 x, quint8 y)
{
    quint16 packedMove = m_packMove(action, x, y);
    m_out << VOF::Command::PlayerMove;
    m_out << packedMove;
    m_tcpSocket->flush();
}
