#include <QtNetwork>
#include <QtCore>
#include <cstring>
#include "VoltOrbFlipServer.h"
#include "GameLogic.h"

VoltOrbFlipServer::VoltOrbFlipServer(QObject *parent)
    : m_pTcpServer(new QTcpServer(this))
{
    connect(m_pTcpServer, &QTcpServer::newConnection,
            this, &VoltOrbFlipServer::slot_attach);

    connect(this, &VoltOrbFlipServer::sig_lobbyUpdate,
            this, &VoltOrbFlipServer::slot_onLobbyStatusUpdate);

    m_startServer(16000);
}

void VoltOrbFlipServer::slot_updateClientsGameState()
{
    for (NWObs* player : std::as_const(m_lobby)) {
        player->updateFullState(m_tGamestate);
    }
}

void VoltOrbFlipServer::slot_onLobbyStatusUpdate(QString szName, quint8 bSlotID, quint8 bLobbyID, bool fIsReady)
{
    for (NWObs* player : std::as_const(m_lobby)) {
        player->sendLobbySlotInfo(szName, bSlotID, fIsReady);
    }
}

void VoltOrbFlipServer::m_broadcastStartMatch()
{
    LOG_OUT << "[SV] Broadcasting start match to all clients" << Qt::endl;
    for (NWObs* player : std::as_const(m_lobby)) {
        player->m_sendStartMatch();
    }
}

void VoltOrbFlipServer::slot_readyStateChanged(quint8 bLobbyID, quint8 bLobbySlotID, bool fIsReady)
{
    m_tGamestate.tPlayerList[bLobbySlotID].fIsReady = fIsReady;
    QString tempName = m_tGamestate.tPlayerList[bLobbySlotID].tIdentity.name; //didn't wanna have it in the signal
    LOG_OUT << "lobby " << (int)bLobbyID
            << " slot " << (int)bLobbySlotID
            << " ready " << (int)fIsReady
            << " tempname " << tempName
            << Qt::endl;
    emit sig_lobbyUpdate(tempName, bLobbySlotID, bLobbyID, fIsReady);
}

void VoltOrbFlipServer::slot_processStartMatch(NWObs *pNWObs)
{
    if (pNWObs->m_getSlotID() == 0) {
        LOG_OUT << "[SV] Host starting match" << Qt::endl;
        m_startMatch(); // Bestehende Server-Logic
        m_broadcastStartMatch();
    }
}

quint8 VoltOrbFlipServer::m_findFirstFreeSlot()
{
    quint8 slotId = 0;
    while(m_lobby.contains(slotId)) {
        slotId++;
    }
    return slotId;
}

quint32 VoltOrbFlipServer::m_getNextFreePlayerId()
{
    return m_playerDatabase.isEmpty() ? 1 : m_playerDatabase.lastKey() + 1;
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

void VoltOrbFlipServer::m_unpackMove(quint8 &action, quint8 &x, quint8 &y, quint16 playerMove)
{
    action = ((playerMove & 0x0F00) >> 8);
    x = ((playerMove & 0x00F0) >> 4);
    y = (playerMove & 0x000F);
    LOG_OUT << " playerMove " << playerMove
            << " action " << action
            << " x " << x
            << " y " << y << Qt::endl;
}

void VoltOrbFlipServer::m_revealTile(quint8 &x, quint8 &y, PlayerSessionState &playerState)
{
    quint8 pos = ((y * VOF::COLUMN_LENGTH) + x);
    playerState.fRevealed[pos] = true;
}

quint8 VoltOrbFlipServer::m_calculatePoints(quint8 &x, quint8 &y, PlayerSessionState &playerState)
{
    quint8 res = 0;
    quint8 pos = ((y * VOF::COLUMN_LENGTH) + x);
    quint8 tileValue = playerState.bBoard[pos];
    if(tileValue != VOF::Tile::Bomb)
    {
        if(playerState.bCurrentScore != 0){
            res = playerState.bCurrentScore * tileValue;
        }
        else{
            res  = tileValue;
        }
    }
    else{
        LOG_OUT << "bomb" << Qt::endl;
    }
    playerState.bCurrentScore = res;
    return res;
}

void VoltOrbFlipServer::m_handleBombHit(PlayerSessionState &player)
{
    player.bCurrentScore = 0;
    player.bombStreak++;

    if (player.bombStreak >= 3)
    {
        // Drop the player down one level after 3 consecutive bomb hits
        if (player.bLevel > 1)
            player.bLevel--;

        player.bombStreak = 0;
        LOG_OUT << "[SV] 3 bombs in a row! Level decreased to " << player.bLevel << Qt::endl;
    }
    else
    {
        LOG_OUT << "[SV] Bomb hit! Current streak: " << player.bombStreak << Qt::endl;
    }
}

void VoltOrbFlipServer::m_startServer(quint16 port)
{
    QHostAddress ipAddress;
    const QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (const QHostAddress &entry : ipAddressesList) {
        if (entry != QHostAddress::LocalHost && entry.toIPv4Address()) {
            //ipAddress = entry;
            break;
        }
    }
    //if we did not find one, use IPv4 localhost
    if (ipAddress.isNull()){
        ipAddress = QHostAddress::LocalHost;
    }

    if (!m_pTcpServer->listen(ipAddress, port)) {
        LOG_ERR << "Unable to start the server: " << m_pTcpServer->errorString() << Qt::endl;
        return;
    }

    LOG_OUT << "[SV] The server is running on IP: " << ipAddress.toString()
            << " Port: " << m_pTcpServer->serverPort() << Qt::endl;
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

    connect(player, &NWObs::sig_readyStateChanged,
            this, &VoltOrbFlipServer::slot_readyStateChanged);

    connect(player, &NWObs::sig_quit,
            this, &VoltOrbFlipServer::slot_detach);

    connect(player, &NWObs::sig_playerMove,
            this, &VoltOrbFlipServer::m_processInput);

    connect(player, &NWObs::sig_startMatchRequested,
            this, &VoltOrbFlipServer::slot_processStartMatch);


    connect(this, &VoltOrbFlipServer::sig_loginSuccessful,
            player, &NWObs::slot_loginSuccessful);

}

void VoltOrbFlipServer::slot_assignLobby(NWObs* pNWObs, quint8 lobbyIDin)
{
    if (m_lobby.count() < VOF::MAX_CLIENTS) {
        quint8 slotID = m_findFirstFreeSlot();
        m_lobby.insert(slotID, pNWObs);
        pNWObs->m_assignSlotID(slotID);

        quint32 playerId = pNWObs->m_getId();
        if (m_playerDatabase.contains(playerId)) {
            PlayerProfile& profile = m_playerDatabase[playerId];
            m_tGamestate.tPlayerList[slotID].tIdentity = profile.identity;
            m_tGamestate.tPlayerList[slotID].fIsReady = false;
        }

        LOG_OUT << "[SV] Welcome Player " << pNWObs->m_getId()
                << " in Lobby " << lobbyIDin
                << " on Lobby Slot " << slotID << Qt::endl;

        for (auto it = m_lobby.begin(); it != m_lobby.end(); ++it) {
            if (it.value() != pNWObs) { // Nicht sich selbst
                quint8 bSlotId = it.key();
                NWObs* existingPlayer = it.value();
                pNWObs->sendLobbySlotInfo(existingPlayer->m_getName(), bSlotId, existingPlayer->m_getReady());
            }
        }
        //LOG_OUT << pNWObs->m_getName() << slotID << Qt::endl;
        emit sig_lobbyUpdate(pNWObs->m_getName(), slotID);
    } else {
        LOG_OUT << "[SV] ERR: Lobby " << lobbyIDin << " Already Full" << Qt::endl;
    }
}

void VoltOrbFlipServer::slot_processHandshake(NWObs* pNWObs, quint32 idIn, quint16 tokenIn)
{
    LOG_OUT << "[SV] INF: Handshake with " << idIn
        << " on " << tokenIn << Qt::endl;

    if(m_playerDatabase.contains(idIn)){ //known ID
        LOG_OUT << "[SV] known ID : " << idIn << Qt::endl;
        if(!m_clients.contains(idIn)){ //currently not connected
            PlayerProfile &existingPlayer = m_playerDatabase[idIn];
            if (existingPlayer.token == tokenIn) {
                LOG_OUT << "[SV] INF: Welcome back " << idIn << Qt::endl;
                existingPlayer.identity.name = pNWObs->m_getName();
                emit sig_loginSuccessful();
                return;
            } else {
                LOG_ERR << "[SV] ERR: Wrong token for slot " << idIn << Qt::endl;
                slot_refreshToken(pNWObs);
                return;
            }
        }//id already in use LOG_OUT << " [SV] id already connected :" << idIn << Qt::endl;
        else{ //first login this server life -> refresh token
            LOG_ERR << "ERR Handshake requested while connected" << Qt::endl;
            slot_refreshToken(pNWObs);
            return;
        }
    } else { // new player
        LOG_OUT << "[SV] unknownID : " << idIn << Qt::endl;
        slot_provideNewLogin(pNWObs);
        return;
    }
}

void VoltOrbFlipServer::slot_provideNewLogin(NWObs* pNWObs)
{
    quint32 dwID = m_getNextFreePlayerId();
    quint16 wToken = m_generateUniqueToken();

    PlayerProfile profile;
    profile.identity.id = dwID;
    profile.identity.name = pNWObs->m_getName();
    profile.token = wToken;
    m_playerDatabase.insert(dwID, profile);
    m_clients.insert(dwID, pNWObs);

    LOG_OUT << "[SV] INF: Welcome " << dwID << " to the server" << Qt::endl;
    pNWObs->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_refreshToken(NWObs* pNWObs)
{
    quint32 dwID = pNWObs->m_getId();
    quint16 wToken = m_generateUniqueToken();

    PlayerProfile profile = m_playerDatabase[dwID];
    profile.token = wToken;
    m_clients.insert(dwID, pNWObs);

    LOG_OUT << "[SV] INF: Welcome " << dwID << " back to the server" << Qt::endl;
    pNWObs->m_updateLogin(dwID, wToken);
}

void VoltOrbFlipServer::slot_detach(NWObs* pNWObs)
{
    LOG_OUT << "[SV] INF: Goodbye " << pNWObs->m_getId() << Qt::endl;
    emit sig_lobbyUpdate("Waiting ...", pNWObs->m_getSlotID(), 0 , 0);
    m_lobby.remove(pNWObs->m_getSlotID());
    m_clients.remove(pNWObs->m_getId());
    pNWObs->deleteLater();
}

void VoltOrbFlipServer::m_processInput(NWObs* pNWObs, quint16 playerMove)
{
    quint8 action, x, y;
    m_unpackMove(action, x, y, playerMove);

    if(action != VOF::Action::Click)
        return;


    quint8 slot = pNWObs->m_getSlotID();
    PlayerSessionState& player = m_tGamestate.tPlayerList[slot];

    m_revealTile(x, y, player);
    bool bombHit    = !(bool)m_calculatePoints(x, y, player);
    bool levelDone  = !bombHit && GameLogic::FinishLevelIfCompleted(
                         player.bBoard, player.fRevealed,
                         player.bCurrentScore, player.bTotalScore, player.bLevel);

    if(bombHit)
        m_revealBoard(player); // Show full board so client can see where the bomb was

    // Send current state immediately so the client sees the result for 2s
    slot_updateClientsGameState();

    if(levelDone)
        m_checkWinCondition(); // Check win before scheduling next board

    // Schedule board reset after 2s on bomb or level completion
    // On a win, m_handlePlayerWin takes over — no new board needed
    bool needsNewBoard = (bombHit || levelDone) && m_tGamestate.fIsGameRunning;
    if(needsNewBoard)
    {
        QTimer::singleShot(2000, this, [this, slot, bombHit]()
                           {
                               PlayerSessionState& p = m_tGamestate.tPlayerList[slot];
                               if(bombHit)
                               {
                                   p.bCurrentScore = 0;
                                   m_handleBombHit(p);
                               }
                               m_generateBoard(p);
                               slot_updateClientsGameState();
                           });
    }
}

bool VoltOrbFlipServer::m_verifyInput(std::string input)
{
    return !input.empty();
}

void VoltOrbFlipServer::m_applyMove(std::string input)
{
    quint32 playerId = std::stoi(input.substr(0, input.find(':')));
    quint8 tileIndex = std::stoi(input.substr(input.find(':') + 1));

    PlayerSessionState& player = m_tGamestate.tPlayerList[playerId];

    int row = tileIndex / 5;
    int col = tileIndex % 5;

    GameLogic::RevealTileWithScore(
        player.bBoard,
        player.fRevealed,
        row,
        col,
        player.bCurrentScore,
        player.bLevel
        );

    slot_updateClientsGameState();
    m_checkWinCondition();
}

// ===== Generate board =====
void VoltOrbFlipServer::m_generateBoard(PlayerSessionState& player)
{
    //LOG_OUT << "[SV] generating board for player " << player.bSlotId << Qt::endl;
    GameLogic::GenerateField5x5_Level(player.bBoard, player.bLevel);

    for(int i = 0; i < 25; ++i)
    {
        player.fRevealed[i] = false;
    }

    player.bCurrentScore = 0;
    //LOG_OUT << "[SV] board for player " << player.bSlotId << " generated" << Qt::endl;
}

void VoltOrbFlipServer::m_revealBoard(PlayerSessionState& player){
    for(int i = 0; i < 25; ++i)
    {
        player.fRevealed[i] = true;
    }
}

// ===== Start match =====
void VoltOrbFlipServer::m_startMatch()
{
    LOG_OUT << "[SV] starting... " << Qt::endl;
    m_tGamestate.fIsGameRunning = true;

    for(auto& player : m_tGamestate.tPlayerList)
    {
        player.bLevel = 1;
        player.bTotalScore = 0;
        m_generateBoard(player);
    }

    slot_updateClientsGameState();
}


void VoltOrbFlipServer::m_checkWinCondition()
{
    for(auto it = m_tGamestate.tPlayerList.begin(); it != m_tGamestate.tPlayerList.end(); ++it)
    {
        quint8 slot = it.key();
        PlayerSessionState& player = it.value();

        if(player.bTotalScore >= 100)
        {
            m_handlePlayerWin(slot);
            return;
        }
    }
}

void VoltOrbFlipServer::m_handlePlayerWin(quint8 bWinnerSlot)
{
    LOG_OUT << "[SV] Player on Slot " << bWinnerSlot << " wins the match!" << Qt::endl;

    m_tGamestate.fIsGameRunning = false;
    slot_updateClientsGameState();

    // After Player won, return all Players back to menu
    QTimer::singleShot(10000, this, [this]()
    {
        LOG_OUT << "[SV] Returning all players to menu..." << Qt::endl;

        for(auto& player : m_tGamestate.tPlayerList)
        {
            player.bLevel = 1;
            player.bTotalScore = 0;
            player.bCurrentScore = 0;

            for(int i = 0; i < 25; ++i)
            {
                player.fRevealed[i] = false;
                player.bBoard[i] = 0;
            }
        }

        m_tGamestate.fIsGameRunning = false;
        slot_updateClientsGameState();
    });
}
