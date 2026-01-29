#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "GlobalDefines.h"

class IObserver
{
public:
    virtual ~IObserver(){};
    virtual void updateFullState(const GameState &tState) = 0;
    virtual void onPlayerStatusChanged(quint32 dwPlayerId, bool fIsReady) = 0;
    virtual void onTileRevealed(quint32 dwPlayerId, quint8 bTileIndex, quint8 bValue) = 0;
};

#endif // IOBSERVER_H
