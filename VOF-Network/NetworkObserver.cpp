#include <QTextStream>

#include "NetworkObserver.h"


void NetworkObserver::updateStates(const GameState &state)
{
    QByteArray data = prepareDataForThisPlayer(state);
}

QByteArray NetworkObserver::prepareDataForThisPlayer(const GameState &)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << "";
    return block;
}
