#ifndef VOLTORBFLIPSERVER_H
#define VOLTORBFLIPSERVER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTimer>

#include "GlobalDefines.h"
#include "NetworkObserver.h"
#include "PlayerProfile.h"
#include "GameLogic.h"


QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

class VoltOrbFlipServer : public QObject
{
    Q_OBJECT

signals:
    // ===== Signals for client communication =====
    void sig_identificationValid(quint32 dwIDout, quint16 wTokenout);
    void sig_lobbyUpdate(QString szName, quint8 bSlotID, quint8 bLobbyID = 0, bool fIsReady = 0);
    void sig_loginSuccessful();

public:
    explicit VoltOrbFlipServer(QObject *parent = nullptr);

    // ===== Server control =====
    void m_startServer(quint16 port = 0);

    // ===== Player input handling =====
    void m_processInput(NWObs* pNWObs, quint16 playerMove);
    bool m_verifyInput(std::string input);
    void m_applyMove(std::string input);

    // ===== Match / game management =====
    void m_startMatch();
    void m_generateBoard(PlayerSessionState& player);

    // ===== Conversion between internal board representations =====
    Field m_convertBoardToField(const PlayerSessionState& player);
    void m_convertFieldToBoard(PlayerSessionState& player, const Field& field);

    void m_broadcastStartMatch();

public slots:
    // ===== Network / lobby handling =====
    void slot_assignLobby(NWObs* pNWObs, quint8 lobbyIDin = 0);
    void slot_attach();
    void slot_updateClientsGameState();
    void slot_onLobbyStatusUpdate(QString szName, quint8 bSlotID, quint8 bLobbyID = 0, bool fIsReady = 0);
    void slot_readyStateChanged(quint8 bLobbyID, quint8 bLobbySlotID, bool fIsReady);
    void slot_processStartMatch(NWObs * pNWObs);

    // ===== Login / session handling =====
    void slot_processHandshake(NWObs* pNWObs, quint32 ID, quint16 token);
    void slot_provideNewLogin(NWObs* pNWObs);
    void slot_refreshToken(NWObs* pNWObs);
    void slot_detach(NWObs* pNWObs);

private:
    // ===== Network infrastructure =====
    QTcpServer *m_pTcpServer = nullptr;
    QMap<quint32, PlayerProfile> m_playerDatabase;
    QMap<quint32, NWObs*> m_clients;

    // ===== Game state & lobby =====
    GameState m_tGamestate;
    QMap<quint8, NWObs*> m_lobby;

    // ===== Player and token management =====
    quint8 m_findFirstFreeSlot();
    quint32 m_getNextFreePlayerId();
    quint16 m_generateUniqueToken();
    void m_deletePlayerInfo(quint32 playerId);

    // ===== Logging & debugging =====
    void m_logMove(quint32 playerId, std::string input, int result, bool gameOver);

    // ===== Move processing =====
    void m_unpackMove(quint8 &action, quint8 &x, quint8 &y, quint16 playerMove);
    void m_revealTile(quint8 &x, quint8 &y, PlayerSessionState &playerState);
    quint8 m_calculatePoints(quint8 &x, quint8 &y, PlayerSessionState &playerState);

    // ===== Game end / win condition handling =====
    void m_checkWinCondition();
    void m_handlePlayerWin(quint8 bWinnerSlot);
};

#endif // VOLTORBFLIPSERVER_H
