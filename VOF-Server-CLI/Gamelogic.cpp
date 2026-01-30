#include "GameLogic.h"
#include <QRandomGenerator>
#include <QTextStream>

QTextStream out(stdout);
QTextStream err(stderr);

GameLogic::GameLogic() {}

// ===== interne Helfer =====
void GameLogic::PlaceExactValue(Field& Field5x5, QChar Value, int Count)
{
    Q_ASSERT(Count <= 25);
    int Placed = 0;
    while (Placed < Count)
    {
        int Row = QRandomGenerator::global()->bounded(5);
        int Col = QRandomGenerator::global()->bounded(5);

        if (Field5x5[Row][Col] == '\0')
        {
            Field5x5[Row][Col] = Value;
            ++Placed;
        }
    }
}

void GameLogic::FillRemainingWithValue(Field& Field5x5, QChar Value)
{
    for (int Row = 0; Row < 5; ++Row)
    {
        for (int Col = 0; Col < 5; ++Col)
        {
            if (Field5x5[Row][Col] == '\0')
            {
                Field5x5[Row][Col] = Value;
            }
        }
    }
}

// ===== Feld generieren abhängig vom Level =====
Field GameLogic::GenerateField5x5_Level(quint8 bLevel)
{
    Field Field5x5(5, QVector<QChar>(5, '\0'));

    const int MineCount = 4 + bLevel; // Level1=5, L2=6, L3=7

    int TwoCount = 0;
    int ThreeCount = 0;

    switch(bLevel)
    {
    case 1: TwoCount = 2; ThreeCount = 1; break;
    case 2: TwoCount = 3; ThreeCount = 1; break;
    case 3: TwoCount = 3; ThreeCount = 2; break;
    default: TwoCount = 2; ThreeCount = 1; break;
    }

    PlaceExactValue(Field5x5, 'X', MineCount);
    PlaceExactValue(Field5x5, '2', TwoCount);
    PlaceExactValue(Field5x5, '3', ThreeCount);
    FillRemainingWithValue(Field5x5, '1');

    return Field5x5;
}

// ===== Summen & Minen zählen =====
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

    for (int i = 0; i < 5; ++i)
    {
        RowSums[i] = 0;
        ColSums[i] = 0;
        RowMines[i] = 0;
        ColMines[i] = 0;
    }

    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            QChar c = Field5x5[row][col];
            quint8 val = (c == 'X') ? 0 : c.digitValue();

            RowSums[row] += val;
            ColSums[col] += val;

            if (c == 'X')
            {
                RowMines[row]++;
                ColMines[col]++;
            }
        }
    }
}

// ===== Feld ausgeben =====
void GameLogic::PrintField(const Field& Field5x5, const bool fRevealed[5][5])
{
    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            if (fRevealed && !fRevealed[row][col])
                out << "* ";
            else
                out << Field5x5[row][col] << " ";
        }
        out << "\n";
    }
    out << "\n";
}

// ===== Level abgeschlossen prüfen =====
bool GameLogic::IsLevelCompleted(const Field& field, const bool fRevealed[5][5])
{
    for (int row = 0; row < 5; ++row)
    {
        for (int col = 0; col < 5; ++col)
        {
            QChar c = field[row][col];
            if ((c == '2' || c == '3') && !fRevealed[row][col])
                return false;
        }
    }
    return true;
}

// ===== Feld aufdecken + Score + Level-Logik =====
void GameLogic::RevealTileWithScore(Field& field, bool fRevealed[5][5],
                                    int row, int col,
                                    quint8& bCurrentScore,
                                    quint8& bLevel)
{
    if (fRevealed[row][col]) return;

    fRevealed[row][col] = true;
    QChar c = field[row][col];

    if (c == 'X')
    {
        if (bLevel > 1) bLevel--;
        bCurrentScore = 0;
        return;
    }

    quint8 value = static_cast<quint8>(c.digitValue());
    if (bCurrentScore == 0)
        bCurrentScore = value;
    else
        bCurrentScore *= value;

    // Prüfen, ob alle 2er/3er aufgedeckt → Level up
    if (IsLevelCompleted(field, fRevealed))
    {
        if (bLevel < 3) bLevel++;
        bCurrentScore = 0;
    }
}

// ===== Level beenden =====
void GameLogic::NextLevel(quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel)
{
    bTotalScore += bCurrentScore;
    bCurrentScore = 0;
    if (bLevel < 3) bLevel++;
}
