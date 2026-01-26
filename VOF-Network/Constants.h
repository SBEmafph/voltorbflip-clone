#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

namespace VOF {
static const quint8 MAX_CLIENTS = 8;
static const quint16 DEFAULT_PORT = 16000;

enum Command : qint8 {
    LoginRequest    = 0x01,
    LoginSuccess    = 0x02,
    GameStateUpdate = 0x03,
    PlayerMove      = 0x04,
    ErrorMessage    = 0x7F
};


enum ConnectionState {
    WaitingForHandshake,
    Authenticated,
    Closing
};

}

#endif // CONSTANTS_H
