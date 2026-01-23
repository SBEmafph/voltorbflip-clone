#ifndef NETWORKOBSERVER_H
#define NETWORKOBSERVER_H

#include <QTcpSocket>

#include "IObserver.h"

class NetworkObserver : IObserver
{
public:
    NetworkObserver(QTcpSocket* clientSocket) : m_socket(clientSocket){}
    //void setSocket();
    QTcpSocket* getSocket();
    //void saveState();
    void updateStates(const GameState &state);
private:
    QTcpSocket* m_socket;
    QByteArray prepareDataForThisPlayer(const GameState &);
};

#endif // NETWORKOBSERVER_H
