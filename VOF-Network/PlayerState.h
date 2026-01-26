#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include <qtypes.h>
#include <QString.h>

struct PlayerState{
public:
    bool m_ready;
    bool m_fItemEquipped[3];
    bool m_fRevealed[25];
    quint8 m_bBoard[25];
    quint8 m_bPlayerID;
    quint8 m_bLevel;
    quint8 m_bTotalScore;
    quint8 m_bCurrentScore;
    QString m_cPlayerName;
};

#endif // PLAYERSTATE_H
