#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

namespace VOF {
static const quint8 MAX_CLIENTS = 8;
static const quint16 DEFAULT_PORT = 16000;

enum Action : quint8{
    Click = 0,
    Note1 = 1,
    Note2 = 2,
    Note3 = 3,
    NoteBomb = 4,
    Lense = 5
};

enum Command : qint8 {
    LoginRequest    = 0x01,
    NewLoginConfig,
    LoginSuccess,
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

#endif // CONSTANTS_H
