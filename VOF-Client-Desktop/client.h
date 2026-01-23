#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDataStream>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
public slots:
    void m_attach();
signals:

private:
    QTcpSocket *m_tcpSocket = nullptr;
    QDataStream m_in;
};

#endif // CLIENT_H
