#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>

#include "GlobalDefines.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class Client : public QObject
{
    Q_OBJECT
signals:
    void sig_identificationSent();
    //void sig_matchAction(VOF::Action action, quint8 x, quint8 y);
    void sig_newLogin(quint32 id, quint16 token, const QString& name, bool force);
    void sig_newLoginSaved();
    void sig_connected();
    void sig_lobbySelected();
    void sig_lobbyUpdate(const QString& name, quint8 bSlotID, bool fIsReady);
    void sig_setHostState(bool isHost);
    void sig_gameStateUpdate(const GameState& gameState, quint8 ownSlotID);
    void sig_matchStarted();

public slots:
    void on_ReadyRead();
    void on_attach(QHostAddress ipAdressIn = QHostAddress::LocalHost, quint16 port = 16000);
    void on_detach();

    void m_readLobbyState();
    bool m_checkStartLobby();

    void slot_changeReadyState();
    void slot_joinSelectedLobby();
    void slot_sendStartMatch();
    void slot_sendIdentification();
    void slot_sendMatchAction(VOF::Action action, quint8 x, quint8 y);
    void slot_updateConfig(quint32 ID = 0, quint16 token = 0, QString name = "", bool force = false);
    void m_updateLobbyState(const QString& name, quint8 bSlotID, bool fIsReady);

public:
    explicit Client(QObject *parent = nullptr);
    bool m_updateInternalConfig(
        quint32 ID = 0,
        quint16 token = 0,
        QString name = "",
        bool force = false
    );

    QTcpSocket* m_getTcpSocket() { return m_pTcpSocket; };
    void m_setTcpSocket(QTcpSocket * sIn) { m_pTcpSocket = sIn; m_pTcpSocket->setParent(this); }

    quint32 m_getID() { return m_dwID; }
    void m_setID(quint32 dwIDin) { m_dwID = dwIDin; }

    quint16 m_getToken() { return m_wToken; }
    void m_setToken(quint16 wTokenin) { m_wToken = wTokenin; }

    QString m_getName() { return m_szName; }
    void m_setName(QString szNamein) { m_szName = szNamein; }

    quint8 m_getLobbyID() { return m_bLobbySlotID; }
    void m_setLobbyID(quint8 bLobbyIdIn) { m_bLobbySlotID = bLobbyIdIn; }

private:
    QTcpSocket *m_pTcpSocket = nullptr;
    QDataStream m_in;
    QDataStream m_out;

    quint32 m_dwID;
    quint16 m_wToken;
    QString m_szName;
    quint8 m_bLobbyID = 0;
    quint8 m_bLobbySlotID;
    bool m_fReady = false;
    bool m_fInLobby = false;

    GameState m_tGamestate;

    void m_loadConfig(bool force = false);
    quint16 m_packMove(VOF::Action action, quint8 x, quint8 y);
};

#endif // CLIENT_H
