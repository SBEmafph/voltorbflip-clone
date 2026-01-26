#ifndef VOLTORBFLIPSERVER_H
#define VOLTORBFLIPSERVER_H

#include <QObject>
#include <QList>
#include <QString>

#include "GameState.h"
#include "NetworkObserver.h"


QT_BEGIN_NAMESPACE
class QTcpServer;
class QTcpSocket;
QT_END_NAMESPACE

class VoltOrbFlipServer : public QObject
{
    Q_OBJECT

public:
    explicit VoltOrbFlipServer(QObject *parent = nullptr);
    void m_startServer(quint16 port = 0);
    void m_detach(NetworkObserver* obs);
    void m_processInput(int playerId, std::string Input);
    bool m_verifyInput(std::string input);
    void m_applyMove(std::string input);

public slots:
    void m_processHandshake(quint32 ID, quint16 token);
    void m_attach();
    void m_notifyClients();

private:
    quint8 m_findFirstFreeSlot();
    QTcpServer *m_tcpServer = nullptr;

    GameState m_gamestate;
    QMap<quint8, NetworkObserver*> m_clients;

    void deletePlayerInfo(quint32 playerId);

};

#endif // VOLTORBFLIPSERVER_H
