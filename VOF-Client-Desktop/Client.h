#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>

#include "Constants.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class Client : public QObject
{
    Q_OBJECT
signals:
    void sig_identificationSent();
    void sig_matchAction(VOF::Action action, quint8 x, quint8 y);
    void sig_newLoginSaved();

public:
    explicit Client(QObject *parent = nullptr);

public slots:
    void slot_onReadyRead();
    void slot_attach(QHostAddress ipAdressIn = QHostAddress::LocalHost, quint16 port = 16000);
    void slot_detach();
    void m_readLobbyState();
    bool m_checkStartLobby();
    void m_sendIdentification();

    void m_updateConfig(bool force = false);

    QTcpSocket* m_getTcpSocket() { return m_tcpSocket; };
    void m_setTcpSocket(QTcpSocket * sIn) { m_tcpSocket = sIn; m_tcpSocket->setParent(this); }

    quint32 m_getID() { return m_dwID; }
    void m_setID(quint32 dwIDin) { m_dwID = dwIDin; }

    quint16 m_getToken() { return m_wToken; }
    void m_setToken(quint16 wTokenin) { m_wToken = wTokenin; }

    QString m_getName() { return m_szName; }
    void m_setName(QString szNamein) { m_szName = szNamein; }

private slots:
    void slot_sendMatchAction(VOF::Action action, quint8 x, quint8 y);

private:
    QTcpSocket *m_tcpSocket = nullptr;
    QDataStream m_in;
    QDataStream m_out;

    quint32 m_dwID;
    quint16 m_wToken;
    QString m_szName;

    void m_loadConfig(bool force = false);
    quint16 m_packMove(VOF::Action action, quint8 x, quint8 y);
};

#endif // CLIENT_H
