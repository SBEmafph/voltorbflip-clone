#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <QtTypes>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QtGlobal>

extern QTextStream err;
extern QTextStream out;

#define __FILENAME__ (strrchr(__FILE__, '/') ?  strrchr(__FILE__, '/') + 1 : \
(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : \
 __FILE__))

#define LOG_OUT out << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "] " \
<< "[" << __FILENAME__ << ":" << __LINE__ << " " << __FUNCTION__ << "] -> "

#define LOG_ERR err << "[" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << "] " \
<< "!!! ERROR in " << "[" << __FILENAME__ << ":" << __LINE__ << " " << __FUNCTION__ << "] -> "

namespace VOF {

// Maximum number of players in a lobby
static const quint8 MAX_CLIENTS = 8;

// Board dimensions
static const quint8 ROW_LENGTH = 5;
static const quint8 COLUMN_LENGTH = 5;

// Server default port
static const quint16 DEFAULT_PORT = 16000;

// Tile constants
static const quint8 TILE_MINE = 0;        // Mine tile
static const quint8 TILE_MIN_VALUE = 1;   // Minimum value tile
static const quint8 TILE_MAX_VALUE = 9;   // Maximum value tile

// Player actions
enum Action : quint8 {
    Click = 0,
    Note1 = 1,
    Note2 = 2,
    Note3 = 3,
    NoteBomb = 4,
    Lense = 5
};

// Commands sent over network
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

// Connection state reference
enum ConnectionState {
    WaitingForHandshake,
    Authenticated,
    Closing
};

} // namespace VOF

// ===== Move information =====
struct Move {
    quint8 tileIndex; // 0-24
    quint8 value;     // Tile value revealed
    quint32 timestamp;
    bool wasGameOver;
};

// ===== Player identity =====
struct PlayerIdentity {
    quint32 id;
    QString name;
};

// ===== Player session state =====
struct PlayerSessionState {
    PlayerIdentity tIdentity;
    bool fIsReady = false;
    quint8 bSlotId;

    // Game data only during match
    quint8 bBoard[(VOF::ROW_LENGTH * VOF::COLUMN_LENGTH) - 1]; // 25 tiles
    bool fRevealed[(VOF::ROW_LENGTH * VOF::COLUMN_LENGTH) - 1]; // revealed flags
    quint8 bCurrentScore = 0;
    quint8 bTotalScore = 0;
    quint8 bLevel = 1;
    bool fItemEquipped[3];

    QList<Move> currentMatchMoves;
};

// ===== Game state =====
struct GameState {
    quint8 bLobbyId;
    QMap<quint8, PlayerSessionState> tPlayerList;
    bool fIsGameRunning = false;
};

// ===== Replay structure =====
struct MatchReplay {
    QDateTime date;
    QMap<quint32, PlayerIdentity> participants;
    QList<Move> allMovesSortedByTime;
};

#endif // GLOBALDEFINES_H
