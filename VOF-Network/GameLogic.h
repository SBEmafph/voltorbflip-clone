#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QString>
#include <QVector>

using Field = QVector<quint8>;

class GameLogic
{
public:
    GameLogic();

    // Generate field
    static Field GenerateField5x5_Level(quint8 bLevel);

    // Calculate sums and mine counts
    static void CalculateSumsAndMines(const Field& Field5x5, QVector<quint8>& RowSums, QVector<quint8>& ColSums, QVector<quint8>& RowMines, QVector<quint8>& ColMines);

    // Print field
    static void PrintField(const Field& Field5x5, const bool fRevealed[25] = nullptr);

    // Level logic
    static bool IsLevelCompleted(const Field& field, const bool fRevealed[25]);
    static void RevealTileWithScore(Field& field, bool fRevealed[25], int row, int col, quint8& bCurrentScore, quint8& bLevel);
    static bool FinishLevelIfCompleted(const Field& field, const bool fRevealed[25], quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel);

    // Replay logging
    static void ReplayStartGame(const QString& filePath, int gameId);
    static void ReplayLogAction(const QString& filePath, int gameId, int action, int level, int currentScore, int totalScore, int row, int col);
    static void ReplayEndGame(const QString& filePath, int gameId, int level, int totalScore);
    static void ResetReplayFile(const QString& filePath);

private:
    // Placing and initializing tile values
    static void PlaceExactValue(Field& Field5x5, quint8 Value, int Count);
    static void FillRemainingWithValue(Field& Field5x5, quint8 Value);
};

#endif // GAMELOGIC_H
