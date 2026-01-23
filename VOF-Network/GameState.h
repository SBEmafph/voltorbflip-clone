#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QMap>

#include "PlayerState.h"

class GameState {
public:
    QMap<quint8, PlayerState> PlayerList;
private:
};


#endif // GAMESTATE_H
