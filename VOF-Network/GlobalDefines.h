#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <QtTypes>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QTextStream>

extern QTextStream err;
extern QTextStream out;

#define __FILENAME__ (strrchr(__FILE__, '/') ?  strrchr(__FILE__, '/') + 1 : \
(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
 __FILE__))

#define LOG_OUT out << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "] " \
<< "[" << __FILENAME__ << ":" << __LINE__ << " " << __FUNCTION__ << "] -> "

#define LOG_ERR err << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "] " \
<< "!!! ERROR in " << "[" << __FILENAME__ << ":" << __LINE__ << " " << __FUNCTION__ << "] -> "

struct Move {
    quint8 tileIndex; // 0-24
    quint8 value;     // Was dort lag (0-3)
    quint32 timestamp;
    bool wasGameOver;
};

struct PlayerIdentity {
    quint32 id;
    QString name;
};

struct PlayerSessionState {
    PlayerIdentity tIdentity;
    bool fIsReady = false;
    quint8 bSlotId;

    // Gamedata only during game
    quint8 bBoard[25];
    bool fRevealed[25];
    quint8 bCurrentScore = 0;
    quint8 bTotalScore = 0;
    quint8 bLevel = 0;
    bool fItemEquipped[3];

    QList<Move> currentMatchMoves;
};

struct GameState {
    quint8 bLobbyId;
    QMap<quint8, PlayerSessionState> tPlayerList;
    bool fIsGameRunning = false;
};

struct MatchReplay {
    QDateTime date;
    QMap<quint32, PlayerIdentity> participants;
    QList<Move> allMovesSortedByTime;
};

#include <QDateTime>
#include <QtGlobal>

namespace VOF {
static const quint8 MAX_CLIENTS = 8;
static const quint8 ROW_LENGTH = 5;
static const quint8 COLUMN_LENGTH = 5;
static const quint16 DEFAULT_PORT = 16000;

enum Action : quint8{
    Click = 0,
    Note1 = 1,
    Note2 = 2,
    Note3 = 3,
    NoteBomb = 4,
    Lense = 5
};

enum Tile : quint8{
    One = 1,
    Two = 2,
    Three = 3,
    Bomb = 4
};

enum Command : qint8 {
    LoginRequest    = 0x01,
    NewLoginConfig,
    LoginSuccess,
    LobbyJoinRequest,
    LobbyUpdate,
    GameStateUpdate,
    PlayerMove,
    Quit,
    ErrorMessage    = 0x7F
};


enum ConnectionState { //just for reference, not used
    WaitingForHandshake,
    Authenticated,
    Closing
};

}

#endif // GLOBALDEFINES_H
