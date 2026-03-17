#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "GlobalDefines.h"

#include <QString>
#include <QVector>

//using Field = QVector<quint8>;

class GameLogic
{
public:
    GameLogic();

    // Generate field
    static void GenerateField5x5_Level(quint8 (&field)[VOF::TILE_COUNT], quint8 bLevel);

    // Calculate sums and mine counts
    static void CalculateSumsAndMines(const quint8 (&field)[VOF::TILE_COUNT], QVector<quint8>& RowSums, QVector<quint8>& ColSums, QVector<quint8>& RowMines, QVector<quint8>& ColMines);

    // Print field
    static void PrintField(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT] = nullptr);

    // Level logic
    static bool IsLevelCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT]);
    static void RevealTileWithScore(const quint8 (&field)[VOF::TILE_COUNT], bool fRevealed[VOF::TILE_COUNT], int row, int col, quint8& bCurrentScore, quint8& bLevel);
    static bool FinishLevelIfCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT], quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel);

    // Replay logging
    static void ReplayStartGame(const QString& filePath, int gameId);
    static void ReplayLogAction(const QString& filePath, int gameId, int action, int level, int currentScore, int totalScore, int row, int col);
    static void ReplayEndGame(const QString& filePath, int gameId, int level, int totalScore);
    static void ResetReplayFile(const QString& filePath);

private:
    // Placing and initializing tile values
    static void PlaceExactValue(quint8 (&field)[VOF::TILE_COUNT], quint8 Value, int Count);
    static void FillRemainingWithValue(quint8 (&field)[VOF::TILE_COUNT], quint8 Value);
};

#endif // GAMELOGIC_H
