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

    /** @brief Generate 5x5 game field with bombs and value tiles
     *
     *  Generate a 5x5 Field with every Field starting with the value 1
     *  Amount of mines, Value 2 und 3 count tiles are scaling depending on the current level
     *  They randomly replace count 1 tiles.
     */
    static void GenerateField5x5_Level(quint8 (&field)[VOF::TILE_COUNT], quint8 bLevel);

    /** @brief Calculate sums and mine counts for each row and collum
     *
     *  Initialize the size and content of each row/collum field
     *  Then going through each row/collum and count the amount of mines and the sum of the value tiles
     */
    static void CalculateSumsAndMines(const quint8 (&field)[VOF::TILE_COUNT], QVector<quint8>& RowSums, QVector<quint8>& ColSums, QVector<quint8>& RowMines, QVector<quint8>& ColMines);

    /** @brief Print 5x5 field for test purposes
     *
     *  Test Function to view the content of the 5x5 field
     */
    static void PrintField(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT] = nullptr);

    // Level logic
    /** @brief Check if the current level has been completed
     *
     *  Check if the player has revealed all value 2 and 3 tiles to mark of the level completion
     */
    static bool IsLevelCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT]);
    /** @brief Reveal a tile and update the current score
    *
    *  Reveals the selected tile of the row/column if it is not already revealed
    *  If the tile contains a mine, reset the current score to 0
    *  Otherwise, the tile value is multiplied with the current score
    */
    static void RevealTileWithScore(const quint8 (&field)[VOF::TILE_COUNT], bool fRevealed[VOF::TILE_COUNT], int row, int col, quint8& bCurrentScore, quint8& bLevel);
    /** @brief Finish the current level and jump to the next
    *
    *  If the current level has been completed then add the current score to the total score
    *  Reset the current score back to 0 for the next level
    *  Increase the level by 1 up to a maximum of 3
    */
    static bool FinishLevelIfCompleted(const quint8 (&field)[VOF::TILE_COUNT], const bool fRevealed[VOF::TILE_COUNT], quint8& bCurrentScore, quint8& bTotalScore, quint8& bLevel);

    // Replay logging
    /** @brief Initialize replay file for a new game
    *
    *  Ensuring that the replay file contains the header
    *  If not, it will be created with the needed header
    *  The file gets opened and closed to ensure the availability of it
    */
    static void ReplayStartGame(const QString& filePath, int gameId);
    /** @brief Initialize replay file for a new game
    *
    *  Ensuring that the replay file contains the header
    *  If not, it will be created with the needed header
    *  The file gets opened and closed to ensure the availability of it
    */
    static void ReplayLogAction(const QString& filePath, int gameId, int action, int level, int currentScore, int totalScore, int row, int col);
    /** @brief Log end of game
    *
    *  Write a final entry to help indicate the end of a game
    */
    static void ReplayEndGame(const QString& filePath, int gameId, int level, int totalScore);
    /** @brief Reset content inside the .csv
    *
    *  Opens the replay file and deletes all existing content
    *  Then write the default header back into it
    */
    static void ResetReplayFile(const QString& filePath);
};

#endif // GAMELOGIC_H
