#ifndef NETWORKOBSERVER_H
#define NETWORKOBSERVER_H

#include <QTcpSocket>

#include "IObserver.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class NWObs :  public QObject, public IObserver
{
    Q_OBJECT
signals:
    void sig_identificationReceived(NWObs * pNWObs, quint32 id, quint16 token);
    void sig_newAccountRequested(NWObs * pNWObs);
    void sig_quit(NWObs * pNWObs);
    void sig_lobbyRequest(NWObs * pNWObs, quint8 lobby);
    void sig_playerMove(NWObs * pNWObs, quint16 packedMove );

public slots:
    void slot_onReadyRead();
    void slot_loginSuccessful();

public:
    NWObs(QTcpSocket* clientSocket, QObject* parent);
    void updateFullState(const GameState &state);
    void onPlayerReadyState(quint32 dwPlayerId, bool fIsReady);
    void onTileRevealed(quint32 dwPlayerId, quint8 bTileIndex, quint8 bValue);

    void m_proceedToMatch();
    //void saveState();

    void m_reconnect(QTcpSocket* SocketIn);
    void m_requestIdentification();

    void m_updateLogin(quint32 dwIDin, quint16 wTokenIn);

    void m_setSocket(QTcpSocket* socketIn) { m_pTcpSocket = socketIn; m_pTcpSocket->setParent(this); }
    QTcpSocket* m_getSocket() { return m_pTcpSocket; }

    quint8 m_getSlotId() const { return m_bSlotID; }
    void m_setSlotId(quint8 bSlotIDin) { m_bSlotID = bSlotIDin; }

    quint16 m_getToken() const { return m_wToken; }
    void m_setToken(quint16 wTokenIn) { m_wToken = wTokenIn; }

    quint32 m_getId() const { return m_dwID; }
    void m_setId(quint32 dwIDin) { m_dwID = dwIDin; }
    
    void onPlayerStatusChanged(quint32 dwIDin, bool fIsReady);
private:
    quint8 m_blobbyID;
    quint8 m_bSlotID;
    quint16 m_wToken;
    quint32 m_dwID;
    QDataStream m_in;
    QDataStream m_out;
    QTcpSocket* m_pTcpSocket = nullptr;

    QByteArray m_prepareDataForPlayer(const GameState &);
};

#endif // NETWORKOBSERVER_H
