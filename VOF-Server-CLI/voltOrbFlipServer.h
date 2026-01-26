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

static const quint8 MAX_CLIENTS = 8;

class VoltOrbFlipServer : public QObject
{
    Q_OBJECT

public:
    explicit VoltOrbFlipServer(QObject *parent = nullptr);
    void startServer(quint16 port = 0);
    void detach(NetworkObserver* obs);
    void processInput(int playerId, std::string Input);
    bool verifyInput(std::string input);
    void applyMove(std::string input);

public slots:
    void attach();
    void notifyClients();

private:
    quint8 m_findFirstFreeSlot();
    QTcpServer *m_tcpServer = nullptr;

    GameState m_gamestate;
    QMap<quint8, NetworkObserver*> m_clients;

};

#endif // VOLTORBFLIPSERVER_H
