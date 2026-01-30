#include "GameLogic.h"
#include <QRandomGenerator>
#include <QTextStream>
#include <iostream>

QTextStream out(stdout);
QTextStream err(stderr);

GameLogic::GameLogic() {}

// ===== Placing and initializing tile values =====
void GameLogic::PlaceExactValue(Field& Field5x5, quint8 Value, int Count)
{
    Q_ASSERT(Count <= 25);

    int Placed = 0;
    while (Placed < Count)
    {
        int Index = QRandomGenerator::global()->bounded(25);
        if (Field5x5[Index] != VOF::TILE_MINE && Field5x5[Index] != 0)
            continue;

        if (Field5x5[Index] == VOF::TILE_MINE)
        {
            Field5x5[Index] = Value;
            ++Placed;
        }
        else if(Field5x5[Index] == 0)
        {
            Field5x5[Index] = Value;
            ++Placed;
        }
    }
}

void GameLogic::FillRemainingWithValue(Field& Field5x5, quint8 Value)
{
    for (int i = 0; i < 25; ++i)
    {
        if (Field5x5[i] == 0)
            Field5x5[i] = Value;
    }
}

// ===== Generate field depending on level =====
Field GameLogic::GenerateField5x5_Level(quint8 bLevel)
{
    Field Field5x5(25, VOF::TILE_MIN_VALUE);

    const int MineCount = 4 + bLevel; // Level1=5, Level2=6, Level3=7

    // Place Mines
    int placed = 0;
    while(placed < MineCount)
    {
        int idx = QRandomGenerator::global()->bounded(25);
        if(Field5x5[idx] != VOF::TILE_MINE)
        {
            Field5x5[idx] = VOF::TILE_MINE;
            placed++;
        }
    }

    // Place 2 and 3 values
    int twoCount = (bLevel == 1) ? 2 : 3;
    int threeCount = (bLevel == 3) ? 2 : 1;

    placed = 0;
    while(placed < twoCount)
    {
        int idx = QRandomGenerator::global()->bounded(25);
        if(Field5x5[idx] == VOF::TILE_MIN_VALUE)
        {
            Field5x5[idx] = 2;
            placed++;
        }
    }

    placed = 0;
    while(placed < threeCount)
    {
        int idx = QRandomGenerator::global()->bounded(25);
        if(Field5x5[idx] == VOF::TILE_MIN_VALUE)
        {
            Field5x5[idx] = 3;
            placed++;
        }
    }

    return Field5x5;
}

// ===== Calculate sums and mine counts =====
void GameLogic::CalculateSumsAndMines(const Field& Field5x5,
                                      QVector<quint8>& RowSums,
                                      QVector<quint8>& ColSums,
                                      QVector<quint8>& RowMines,
                                      QVector<quint8>& ColMines)
{
    RowSums.resize(5);
    ColSums.resize(5);
    RowMines.resize(5);
    ColMines.resize(5);

    for(int i=0;i<5;i++)
    {
        RowSums[i] = 0;
        ColSums[i] = 0;
        RowMines[i] = 0;
        ColMines[i] = 0;
    }

    for(int row=0; row<5; row++)
    {
        for(int col=0; col<5; col++)
        {
            int idx = row*5 + col;
            quint8 val = Field5x5[idx];

            if(val == VOF::TILE_MINE)
            {
                RowMines[row]++;
                ColMines[col]++;
            }
            else
            {
                RowSums[row] += val;
                ColSums[col] += val;
            }
        }
    }
}

// ===== Print field =====
void GameLogic::PrintField(const Field& Field5x5, const bool fRevealed[25])
{
    for(int row=0; row<5; row++)
    {
        for(int col=0; col<5; col++)
        {
            int idx = row*5 + col;
            if(fRevealed && !fRevealed[idx])
                out << "* ";
            else
                out << (Field5x5[idx] == VOF::TILE_MINE ? "X" : QString::number(Field5x5[idx])) << " ";
        }
        out << "\n";
    }
    out << "\n";
}

// ===== Check if level is completed =====
bool GameLogic::IsLevelCompleted(const Field& field, const bool fRevealed[25])
{
    for(int row=0; row<5; row++)
    {
        for(int col=0; col<5; col++)
        {
            int idx = row*5 + col;
            quint8 val = field[idx];
            if((val==2 || val==3) && !fRevealed[idx])
                return false;
        }
    }
    return true;
}

// ===== Reveal tile, update score and level =====
void GameLogic::RevealTileWithScore(Field& field, bool fRevealed[25], int row, int col, quint8& bCurrentScore, quint8& bLevel)
{
    int idx = row*5 + col;

    if(fRevealed[idx]) return;

    fRevealed[idx] = true;

    quint8 value = field[idx];

    if(value == VOF::TILE_MINE)
    {
        bCurrentScore = 0;
        if(bLevel>1) bLevel--;
        return;
    }

    if(bCurrentScore == 0)
        bCurrentScore = value;
    else
        bCurrentScore *= value;

    if(IsLevelCompleted(field, fRevealed))
    {
        if(bLevel < 3) bLevel++;
        bCurrentScore = 0;
    }
}

// ===== Finish level =====
void GameLogic::NextLevel(quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel)
{
    bTotalScore += bCurrentScore;
    bCurrentScore = 0;

    if(bLevel < 3) bLevel++;
}
