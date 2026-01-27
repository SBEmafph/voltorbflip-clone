#ifndef VOLTORBFLIPSERVER_H
#define VOLTORBFLIPSERVER_H

#include <QObject>
#include <QList>
#include <QString>

#include "GameState.h"
#include "NetworkObserver.h"
#include "PlayerProfile.h"


QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

class VoltOrbFlipServer : public QObject
{
    Q_OBJECT

signals:
    void sig_identificationValid(quint32 dwIDout, quint16 wTokenout);

public:
    explicit VoltOrbFlipServer(QObject *parent = nullptr);
    void m_startServer(quint16 port = 0);
    void m_processInput(int playerId, std::string Input);
    bool m_verifyInput(std::string input);
    void m_applyMove(std::string input);

public slots:
    void slot_assignLobby(NetworkObserver* pNetworkObserver, quint8 lobbyIDin = 0);
    void slot_attach();
    void slot_notifyClients();
    void slot_processHandshake(quint32 ID, quint16 token);
    void slot_provideNewLogin(NetworkObserver* pNetworkObserver);
    void slot_refreshToken(NetworkObserver* pNetworkObserver);
    void slot_detach(NetworkObserver* obs);

private:
    QTcpServer *m_pTcpServer = nullptr;
    QMap<quint32, PlayerProfile> m_playerDatabase;
    QMap<quint32, NetworkObserver*> m_clients;

    GameState m_tGamestate;
    QMap<quint8, NetworkObserver*> m_lobby;

    quint8 m_findFirstFreeSlot();
    quint32 m_getNextFreePlayerId();
    quint16 m_generateUniqueToken();
    void m_deletePlayerInfo(quint32 playerId);
    void m_logMove(quint32 playerId, std::string input, int result, bool gameOver);
};

#endif // VOLTORBFLIPSERVER_H
