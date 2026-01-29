#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include <QString>
#include <QList>
#include <QDateTime>

#include "GlobalDefines.h"

struct PlayerProfile {
    PlayerIdentity identity;
    quint16 token;
    QMap<quint8, Move> everyMatch;
};

#endif // PLAYERPROFILE_H
