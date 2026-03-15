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
    void sig_newLoginSaved();
    void sig_readyStateChanged(quint8 bLobbyID, quint8 bLobbySlotID, bool fIsReady);
    void sig_startMatchRequested(NWObs * pNWObs);

public slots:
    void slot_onReadyRead();
    void slot_loginSuccessful();

public:
    NWObs(QTcpSocket* clientSocket, QObject* parent);
    void updateFullState(const GameState &state);
    void slot_PlayerReadyState(quint32 dwPlayerId, bool fIsReady);
    void onTileRevealed(quint32 dwPlayerId, quint8 bTileIndex, quint8 bValue);

    void m_proceedToMatch();
    //void saveState();

    void m_reconnect(QTcpSocket* SocketIn);
    void m_requestIdentification();
    void m_sendStartMatch();

    void m_updateLogin(quint32 dwIDin, quint16 wTokenIn);

    void m_setSocket(QTcpSocket* socketIn) { m_pTcpSocket = socketIn; m_pTcpSocket->setParent(this); }
    QTcpSocket* m_getSocket() { return m_pTcpSocket; }


    void m_assignSlotID(quint8 bSlotIDin);

    quint8 m_getSlotID() const { return m_bLobbySlotID; }
    void m_setSlotID(quint8 bSlotIDin) { m_bLobbySlotID = bSlotIDin; }

    quint32 m_getId() const { return m_dwID; }
    void m_setId(quint32 dwIDin) { m_dwID = dwIDin; }
    
    quint16 m_getToken() const { return m_wToken; }
    void m_setToken(quint16 wTokenIn) { m_wToken = wTokenIn; }

    QString m_getName() const { return m_szName; }
    void m_setName(QString szNameIn) { m_szName = szNameIn; }

    void sendLobbySlotInfo(QString szName, quint8 bSlotID, bool fIsReady);

    bool m_getReady() const { return m_fReady; }
    void m_setfReady(bool fReadyIn) { m_fReady = fReadyIn; }

private:
    QTcpSocket* m_pTcpSocket = nullptr;
    QDataStream m_in;
    QDataStream m_out;

    quint32 m_dwID;
    quint16 m_wToken;
    QString m_szName;
    quint8 m_bLobbyID;
    quint8 m_bLobbySlotID;
    bool m_fReady = false;

    bool m_waitNewLoginAck = false;
};

#endif // NETWORKOBSERVER_H
