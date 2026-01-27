#ifndef PLAYERPROFILE_H
#define PLAYERPROFILE_H

#include <QString>
#include <QList>
#include <QDateTime>

struct MoveRecord {
    QString timestamp;
    QString input;
    int resultScore;
    bool wasGameOver;
};

struct PlayerProfile {
    quint32 id;
    quint16 token;
    QString name;
    QList<MoveRecord> moveHistory;
};

#endif // PLAYERPROFILE_H
