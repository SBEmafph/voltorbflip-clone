#include "GameLogic.h"

#include <QRandomGenerator>
#include <QTextStream>
#include <QFile>
#include <QDateTime>

static void writeReplayHeaderIfNeeded(const QString& filePath)
{
    QFile file(filePath);

    if (!file.exists())
    {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << "gameid,action,date,time,level,currentscore,totalscore,field\n";
            file.close();
        }
    }
}

GameLogic::GameLogic() {}

// ===== Placing and initializing tile values =====
void GameLogic::PlaceExactValue(quint8 (&field)[VOF::TILE_COUNT], quint8 Value, int Count)
{
    Q_ASSERT(Count <= VOF::TILE_COUNT);

    int Placed = 0;
    while (Placed < Count)
    {
        int Index = QRandomGenerator::global()->bounded(VOF::TILE_COUNT);
        if (field[Index] != VOF::TILE_MINE && field[Index] != 0)
            continue;

        if (field[Index] == VOF::TILE_MINE)
        {
            field[Index] = Value;
            ++Placed;
        }
        else if(field[Index] == 0)
        {
            field[Index] = Value;
            ++Placed;
        }
    }
}

void GameLogic::FillRemainingWithValue(quint8 (&field)[VOF::TILE_COUNT], quint8 Value)
{
    for (int i = 0; i < VOF::TILE_COUNT; ++i)
    {
        if (field[i] == 0)
            field[i] = Value;
    }
}

// ===== Generate field depending on level =====
void GameLogic::GenerateField5x5_Level(quint8 (&field)[VOF::TILE_COUNT], quint8 bLevel)
{
    //LOG_OUT << "[GL] generating field" << Qt::endl;
    memset(field, VOF::Tile::One, sizeof(field));

    const int MineCount = 4 + bLevel; // Level1=5, Level2=6, Level3=7

    // Place Mines
    int placed = 0;
    while(placed < MineCount)
    {
        int idx = QRandomGenerator::global()->bounded(VOF::TILE_COUNT);
        if(field[idx] != VOF::Tile::Bomb)
        {
            field[idx] = VOF::Tile::Bomb;
            placed++;
        }
    }

    // Place 2 and 3 values
    int twoCount = (bLevel == 1) ? 2 : 3;
    int threeCount = (bLevel == 3) ? 2 : 1;

    placed = 0;
    while(placed < twoCount)
    {
        int idx = QRandomGenerator::global()->bounded(VOF::TILE_COUNT);
        if(field[idx] == VOF::Tile::One)
        {
            field[idx] = VOF::Tile::Two;
            placed++;
        }
    }

    placed = 0;
    while(placed < threeCount)
    {
        int idx = QRandomGenerator::global()->bounded(VOF::TILE_COUNT);
        if(field[idx] == VOF::Tile::One)
        {
            field[idx] = VOF::Tile::Three;
            placed++;
        }
    }
    //LOG_OUT << "[GL] finished generating field" << Qt::endl;
}

// ===== Calculate sums and mine counts =====
void GameLogic::CalculateSumsAndMines(const quint8 (&field)[VOF::TILE_COUNT], QVector<quint8>& RowSums, QVector<quint8>& ColSums,QVector<quint8>& RowMines, QVector<quint8>& ColMines)
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
            quint8 val = field[idx];

            if(val == VOF::Tile::Bomb)
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
void GameLogic::PrintField(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT])
{
    for(int row=0; row<5; row++)
    {
        for(int col=0; col<5; col++)
        {
            int idx = row*5 + col;
            if(fRevealed && !fRevealed[idx])
                out << "* ";
            else
                out << (field[idx] == VOF::TILE_MINE ? "X" : QString::number(field[idx])) << " ";
        }
        out << "\n";
    }
    out << "\n";
}

// ===== Check if level is completed =====
bool GameLogic::IsLevelCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT])
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
void GameLogic::RevealTileWithScore(
    const quint8 (&field)[VOF::TILE_COUNT],
    bool fRevealed[VOF::TILE_COUNT],
    int row,
    int col,
    quint8& bCurrentScore,
    quint8& /* bLevel */)   // Level hier ignorieren
{
    int idx = row * 5 + col;

    if (fRevealed[idx])
        return;

    fRevealed[idx] = true;

    quint8 value = field[idx];

    if (value == VOF::TILE_MINE) {
        bCurrentScore = 0;
        return;
    }

    if (bCurrentScore == 0)
        bCurrentScore = value;
    else
        bCurrentScore *= value;
}

bool GameLogic::FinishLevelIfCompleted(
    const quint8 (&field)[VOF::TILE_COUNT],
    const bool fRevealed[VOF::TILE_COUNT],
    quint8& bCurrentScore,
    quint8& bTotalScore,
    quint8& bLevel)
{
    if (!IsLevelCompleted(field, fRevealed))
        return false;

    bTotalScore += bCurrentScore;
    bCurrentScore = 0;

    if (bLevel < 3)
        bLevel++;

    return true;
}

void GameLogic::ReplayStartGame(const QString& filePath, int gameId)
{
    writeReplayHeaderIfNeeded(filePath);

    QFile file(filePath);
    file.close();
}

void GameLogic::ReplayLogAction(const QString& filePath, int gameId, int action, int level, int currentScore, int totalScore, int row, int col)
{
    QFile file(filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text))
        return;

    QTextStream out(&file);

    QDateTime now = QDateTime::currentDateTime();

    QString field = QString("%1x%2").arg(row + 1).arg(col + 1);

    out << gameId << ","
        << action << ","
        << now.toString("yyyy-MM-dd") << ","
        << now.toString("HH:mm:ss") << ","
        << level << ","
        << currentScore << ","
        << totalScore << ","
        << field
        << "\n";

    file.close();
}

void GameLogic::ReplayEndGame(const QString& filePath, int gameId, int level, int totalScore)
{
    ReplayLogAction(filePath, gameId, 3, level, 0, totalScore, -1, -1);
}

void GameLogic::ResetReplayFile(const QString& filePath)
{
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << "gameid,action,date,time,level,currentscore,totalscore,field\n";
        file.close();
    }
}
