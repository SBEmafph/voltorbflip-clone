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
            // Write replay header if it doesn't exist
            QTextStream out(&file);
            out << "gameid,action,date,time,level,currentscore,totalscore,field\n";
            file.close();
        }
    }
}

GameLogic::GameLogic() {}

void GameLogic::GenerateField5x5_Level(quint8 (&field)[VOF::TILE_COUNT], quint8 bLevel)
{
    //LOG_OUT << "[GL] generating field" << Qt::endl;
    // Set  all Tiles of board to 1 by default
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

    // Define amount of 2 and 3 values
    int twoCount   = QRandomGenerator::global()->bounded(3, 3 + bLevel); // Level1: 3, Level2: 3-4, Level3: 3-5
    int threeCount = QRandomGenerator::global()->bounded(1, 1 + bLevel); // Level1: 1, Level2: 1-2, Level3: 1-3

    // Place value 2 tiles
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

    // Place value 3 tiles
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

void GameLogic::CalculateSumsAndMines(const quint8 (&field)[VOF::TILE_COUNT], QVector<quint8>& RowSums, QVector<quint8>& ColSums,QVector<quint8>& RowMines, QVector<quint8>& ColMines)
{
    // Set size to 5
    RowSums.resize(5);
    ColSums.resize(5);
    RowMines.resize(5);
    ColMines.resize(5);

    // Reset the info field for each row/collum
    for(int i=0;i<5;i++)
    {
        RowSums[i] = 0;
        ColSums[i] = 0;
        RowMines[i] = 0;
        ColMines[i] = 0;
    }

    // Check board and count Points and Bombs for each row/collum
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

void GameLogic::RevealTileWithScore(const quint8 (&field)[VOF::TILE_COUNT], bool fRevealed[VOF::TILE_COUNT], int row, int col, quint8& bCurrentScore, quint8& bLevel)   // Ignore level
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

bool GameLogic::FinishLevelIfCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT], quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel)
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
        // Write replay header
        QTextStream out(&file);
        out << "gameid,action,date,time,level,currentscore,totalscore,field\n";
        file.close();
    }
}
