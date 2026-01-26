#ifndef NETWORKOBSERVER_H
#define NETWORKOBSERVER_H

#include <QTcpSocket>

#include "IObserver.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE


class NetworkObserver :  public QObject, public IObserver
{
    Q_OBJECT
signals:
    void m_identify(quint32 id, quint16 token);

public slots:
    void m_onReadyRead();

public:
    NetworkObserver(QTcpSocket* clientSocket, QObject* parent);
    void m_setSocket(QTcpSocket* socketIn) { m_socket = socketIn; m_socket->setParent(this); }
    QTcpSocket* m_getSocket() { return m_socket; }
    //void saveState();
    void m_proceedToMatch();
    QDataStream m_getDataStream();
    void m_reconnect(QTcpSocket* SocketIn);

    quint16 m_getToken() const { return m_token; }
    void m_setToken(quint16 newToken) { m_token = newToken; }

    void m_updateStates(const GameState &state);
private:
    quint16 m_token;
    QDataStream m_in;
    QDataStream m_out;
    QTcpSocket* m_socket = nullptr;
    QByteArray m_prepareDataForPlayer(const GameState &);
};

#endif // NETWORKOBSERVER_H
