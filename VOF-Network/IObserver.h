#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "GameState.h"

class IObserver
{
public:
    virtual ~IObserver(){};
    virtual void updateStates(const GameState &state) = 0;
};

#endif // IOBSERVER_H
