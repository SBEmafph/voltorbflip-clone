#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "GlobalDefines.h"
#include <QVector>
#include <QChar>

using Field = QVector<QVector<QChar>>;

class GameLogic
{
public:
    GameLogic();

    // ===== Feld generieren =====
    static Field GenerateField5x5_Level(quint8 bLevel);

    // ===== Summen & Minen zählen =====
    static void CalculateSumsAndMines(const Field& Field5x5, QVector<quint8>& RowSums, QVector<quint8>& ColSums, QVector<quint8>& RowMines, QVector<quint8>& ColMines);

    // ===== Feld ausgeben =====
    static void PrintField(const Field& Field5x5, const bool fRevealed[5][5] = nullptr);

    // ===== Level-Logik =====
    static bool IsLevelCompleted(const Field& field, const bool fRevealed[5][5]);
    static void RevealTileWithScore(Field& field, bool fRevealed[5][5], int row, int col, quint8& bCurrentScore, quint8& bLevel);
    static void NextLevel(quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel);

private:
    // ===== interne Helfer =====
    static void PlaceExactValue(Field& Field5x5, QChar Value, int Count);
    static void FillRemainingWithValue(Field& Field5x5, QChar Value);
};

#endif // GAMELOGIC_H
