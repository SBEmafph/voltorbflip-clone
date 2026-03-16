#include <QtNetwork>

#include "Client.h"
#include "Config.h"

Client::Client(QObject *parent)
    : QObject{parent}
    , m_pTcpSocket (new QTcpSocket(this))
    , m_pGameState(std::make_shared<GameState>())

{
    m_in.setDevice(m_pTcpSocket);
    m_in.setVersion(QDataStream::Qt_6_5);
    connect(m_pTcpSocket, &QTcpSocket::readyRead,
            this, &Client::on_ReadyRead);

    m_out.setDevice(m_pTcpSocket);
    m_out.setVersion(QDataStream::Qt_6_5);

    connect(this, &Client::sig_connected,
            this, &Client::slot_joinSelectedLobby);

    connect(this, &Client::sig_newLogin,
            this, &Client::slot_updateConfig);

    connect(this, &Client::sig_newLoginSaved,
            this, &Client::slot_sendIdentification);

    /*
    connect(this, &Client::sig_identificationSent,
            this, &Client::slot_sendIdentification);
    */
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
        slot_updateConfig(m_dwID, m_wToken, m_szName, fForce);
}

quint16 Client::m_packMove(VOF::Action action, quint8 x, quint8 y) {
    return static_cast<quint16>((action << 8) | (x << 4) | y);
    //click '0' on field (2,3) -> 0x0230
}

void Client::on_ReadyRead()
{
    quint8 command;
    quint32 tempID;
    quint16 tempToken;
    QString name;
    quint8 slotID;
    bool fIsReady;

    while (m_pTcpSocket->bytesAvailable() > 0) {
        m_in.startTransaction();

        m_in >> command;

        switch (command){
        case VOF::Command::LoginRequest:{
            //LOG_OUT << "LoginRequest " << Qt::endl;
            if (!m_in.commitTransaction()) return;
            slot_sendIdentification();
            break; }
        case VOF::Command::NewLoginConfig:{
            //LOG_OUT << "NewLoginConfig " << Qt::endl;
            m_in >> tempID;
            m_in >> tempToken;

            if (!m_in.commitTransaction()) return;
            //LOG_OUT << "clientOnReadyRead Prev " << m_dwID << " : " << m_wToken << Qt::endl;
            m_dwID = tempID;
            m_wToken = tempToken;
            //LOG_OUT << "clientOnReadyRead After " << m_dwID << " : " << m_wToken << Qt::endl;
            emit sig_newLogin(m_dwID, m_wToken, m_szName, 0);
            break; }
        case VOF::Command::LoginSuccess: {
            //LOG_OUT << "LoginSuccess " << Qt::endl;
            if (!m_in.commitTransaction()) return;
            emit sig_connected();
            break; }
        case VOF::Command::LobbyJoinRequest: {
            LOG_OUT << "LobbyJoinRequest " << Qt::endl;
            if (!m_in.commitTransaction()) return;
            m_in >> slotID;
            if(slotID < VOF::MAX_CLIENTS){
                if(slotID == 0){
                    emit sig_setHostState(true);
                }
                m_bLobbySlotID = slotID;
            }
            break; }
        case VOF::Command::LobbyUpdate: {
            //LOG_OUT << "LobbyUpdate " << Qt::endl;
            if (!m_in.commitTransaction()) return;
            m_in >> name >> slotID >> fIsReady;
            // LOG_OUT << "ready " << m_fReady << " slot " << slotID << Qt::endl;
            m_updateLobbyState(name, slotID, fIsReady);
            break; }
        case VOF::Command::GameStateUpdate: {
            LOG_OUT << "command " << command << Qt::endl;
            quint8 playerCount;
            m_in >> playerCount;
            LOG_OUT << "playerCount " << playerCount << Qt::endl;

            if (!m_in.commitTransaction()) return;
            //TODO check gamestate and how it gets changed
            m_pGameState->tPlayerList.clear();

            for (int p = 0; p < playerCount; ++p)
            {
                quint8 slotId;

                m_in.startTransaction();

                //LOG_OUT << "slotId " << m_bLobbySlotID
                //        << " player.bCurrentScore " << player.bCurrentScore
                //        << " player.bTotalScore " << player.bTotalScore
                //        << " player.bLevel " << player.bLevel
                //        << Qt::endl;

                m_in >> slotId;
                PlayerSessionState player = m_pGameState->tPlayerList[slotId];

                m_in >> player.tIdentity.name;
                m_in >> player.fIsReady;
                m_in >> player.bCurrentScore;
                m_in >> player.bTotalScore;
                m_in >> player.bLevel;

                //LOG_OUT << "slotId " << m_bLobbySlotID
                //        << " player.bCurrentScore " << player.bCurrentScore
                //        << " player.bTotalScore " << player.bTotalScore
                //        << " player.bLevel " << player.bLevel
                //        << Qt::endl;

                if (!m_in.commitTransaction()) return;

                // Board Arrays initialisieren
                memset(player.bBoard, 0, 25);
                memset(player.fRevealed, false, 25);

                quint8 position;  // Revealed Tiles
                while (true)
                {
                    m_in.startTransaction();
                    m_in >> position;
                    //LOG_OUT << player.bBoard[position] << Qt::endl;

                    if (!m_in.commitTransaction()) return;

                    if (position == 255) break; // End-Marker

                    m_in.startTransaction();
                    quint8 value;
                    m_in >> value;

                    if (!m_in.commitTransaction()) return;

                    if(slotId == m_bLobbySlotID){
                        if(value > 8){
                            player.bBoard[position] = (value - 8);
                            player.fRevealed[position] = true;
                        }
                        else{
                            player.bBoard[position] = value;
                            player.fRevealed[position] = false;
                        }
                    }
                    else{
                        player.bBoard[position] = value;
                        player.fRevealed[position] = true;
                    }
                    LOG_OUT << player.bBoard[position];
                }

                LOG_OUT << Qt::endl;
                m_pGameState->tPlayerList[slotId] = player;
            }

            emit sig_gameStateUpdate(m_bLobbySlotID);
            break; }
        case VOF::Command::StartMatch: {
            LOG_OUT << "Match started by server" << Qt::endl;
            emit sig_matchStarted();
            emit sig_setUpGame(m_bLobbySlotID);
            break; }
        default: {
            if (!m_in.commitTransaction()) return;
            break; }
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

void Client::slot_changeReadyState()
{
    m_fReady = !m_fReady;
    m_pGameState->tPlayerList[m_bLobbySlotID].fIsReady = m_fReady;

//    LOG_OUT << "ready " << m_fReady
//            << " lobby " << m_bLobbyID
//            << " slot " << m_bLobbySlotID << Qt::endl;

    m_out << VOF::Command::LobbyUpdate;
    m_out << m_bLobbyID;
    m_out << m_bLobbySlotID;
    m_out << m_fReady;
}

void Client::on_attach(QHostAddress ipAdressIn, quint16 portIn)
{
    if(!m_pTcpSocket)
        m_setTcpSocket(new QTcpSocket(this));

    QAbstractSocket::SocketState currentState = m_pTcpSocket->state();
    LOG_OUT << "Attempting attach. Current State: " << currentState << Qt::endl;

    if (currentState != QAbstractSocket::ConnectedState) {
        LOG_OUT << "Not Connected. M_attach..." << Qt::endl;
        m_pTcpSocket->connectToHost(ipAdressIn, portIn);

        if (!m_pTcpSocket->waitForConnected(3000)) {
            LOG_OUT << "Could not connect: " << m_pTcpSocket->errorString() << Qt::endl;
            //startServer();
            emit sig_connected(); //debug
            return;
        }
    }
}

void Client::on_detach()
{
    if (!m_pTcpSocket) {
        LOG_OUT << "Detach aborted: m_tcpSocket is nullptr!" << Qt::endl;
        return;
    }
    QAbstractSocket::SocketState currentState = m_pTcpSocket->state();
    LOG_OUT << "Attempting detach. Current State: " << currentState << Qt::endl;

    if (currentState == QAbstractSocket::ConnectedState) {
        LOG_OUT << "Sending Quit-Command before application closes..." << Qt::endl;
        if (m_pTcpSocket->isWritable()) {
            m_out << VOF::Command::Quit;
            m_pTcpSocket->flush();
            m_pTcpSocket->waitForBytesWritten(1000);
        }
        m_pTcpSocket->disconnectFromHost();
    }else if (currentState
        != QAbstractSocket::UnconnectedState) {
        LOG_OUT << "Socket not connected, but busy. Aborting..." << Qt::endl;
        m_pTcpSocket->abort();
    } else {
        LOG_OUT << "Socket already disconnected." << Qt::endl;
    }
    m_fInLobby = false;
}

void Client::slot_sendIdentification()
{
    LOG_OUT << "Connected. Sending Identification" << Qt::endl;
    //only read if values are empty
    if((m_dwID == 0) || (m_wToken == 0) || (m_szName.isEmpty()))
        m_loadConfig();

    quint8 command = VOF::Command::LoginRequest;
    m_out << command;
    m_out << m_dwID;
    m_out << m_wToken;
    m_out << m_szName;
    //m_tcpSocket->flush();
    emit sig_identificationSent();
}

void Client::slot_joinSelectedLobby()
{
    //for testing we will only use one lobby
    if(!m_fInLobby){
        LOG_OUT << "Joining Lobby: " << m_bLobbyID << Qt::endl;
        quint8 command = VOF::Command::LobbyJoinRequest;
        m_out << command;
        m_out << m_dwID;
        m_out << m_wToken;
        m_out << m_bLobbyID;
        //m_tcpSocket->flush();
        m_fInLobby = true;
    }
}

void Client::slot_sendStartMatch()
{
    LOG_OUT << "Host sending start match command" << Qt::endl;
    m_out << VOF::Command::StartMatch;
}

bool Client::m_updateInternalConfig(quint32 dwIDIn, quint16 wtokenIN, QString szNameIn, bool fForce)
{
    bool settingChanged = false;
    QJsonObject config = VOF::loadOrCreateConfig();
    quint32 id = config["playerID"].toInt();
    quint16 token = config["sessionToken"].toInt();
    QString name = config["playerName"].toString();

    if(fForce || (id == 0) || (id != dwIDIn)){
        m_dwID = dwIDIn;
        settingChanged = true;
    }
    else{
        m_dwID = id;
    }

    if(fForce || (token == 0) || (token != wtokenIN)){
        m_wToken = wtokenIN;
        settingChanged = true;
    }
    else{
        m_wToken = token;
    }

    if(fForce || name.isEmpty() || (name != szNameIn)){
        m_szName = szNameIn;
        settingChanged = true;
    }
    else{
        m_szName = name;
    }

    if(settingChanged){
        VOF::writeConfig(m_dwID, m_wToken, m_szName, fForce);
    }
    return settingChanged;
}

void Client::slot_updateConfig(
    quint32 dwIDIn,
    quint16 wtokenIN,
    QString szNameIn,
    bool fForce)
{
    bool settingChanged = m_updateInternalConfig(dwIDIn, wtokenIN, szNameIn, fForce);
    if(settingChanged){
        emit sig_newLoginSaved();
    }
}

void Client::m_updateLobbyState(const QString &name, quint8 bSlotID, bool fIsReady)
{
    LOG_OUT << name
            //<< m_tGamestate->tPlayerList[bSlotID].tIdentity.name
            << Qt::endl;
    m_pGameState->bLobbyId = 0;
    if(name == "Waiting ..."){
        m_pGameState->tPlayerList[bSlotID].tIdentity.name = "";
    }
    else{
        m_pGameState->tPlayerList[bSlotID].tIdentity.name = name;
    }
        m_pGameState->tPlayerList[bSlotID].fIsReady = fIsReady;
    //LOG_OUT << "ready " << m_fReady << " slot " << bSlotID << Qt::endl;

    emit sig_lobbyUpdate(name, bSlotID, fIsReady);
}

void Client::slot_sendMatchAction(VOF::Action action, quint8 x, quint8 y)
{
    LOG_OUT << "action " << action << " x: " << x << " y: " << y << Qt::endl;
    quint16 packedMove = m_packMove(action, x, y);
    LOG_OUT << packedMove << Qt::endl;
    m_out << VOF::Command::PlayerMove;
    m_out << packedMove;
    m_pTcpSocket->flush();
}
