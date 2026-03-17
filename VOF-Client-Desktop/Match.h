#ifndef MATCH_H
#define MATCH_H

#include <QWidget>
#include <QButtonGroup>
#include <QGridLayout>
#include <QTimer>

#include "Replay.h"
#include "CardButton.h"
#include "GlobalDefines.h"

namespace Ui {
class Match;
}

class Match : public QWidget
{
    Q_OBJECT

public:
    explicit Match(QWidget *parent = nullptr);
    ~Match();

    // Core game functions
    /** @brief Initializing UI and values
    *
    *  Setting the current score back to 0
    *  Resetting the 5x5 board to a new one
    *  Updating all nececary widgets
    */
    void m_startLevel();
    /** @brief Update the info fields of each row/collum
    *
    *  Updates the values of the info field of each row/collum to show the according bomb count and sum of points
    */
    void m_updateRowColLabels(
        const QVector<quint8>& RowSums,
        const QVector<quint8>& ColSums,
        const QVector<quint8>& RowMines,
        const QVector<quint8>& ColMines
    );

    void setGameState(std::shared_ptr<GameState> gameState) { m_pGameState = gameState; };

signals:
    void sig_backToMenu();
    void sig_action(VOF::Action action, quint8 x, quint8 y);

public slots:
    /** @brief Quit current Match
    *
    *  Resets game back to default values before sending you back to the menu
    */
    void on_quitBtn_clicked();
    void on_openMemoButtons();
    void on_closeMemoButtons();
    /** @brief Update the game board for all players
    *
    *  Goes through all players in the current game state and updates
    *  the visible tiles for each player
    *  The local player uses m_updatePlayerTile to refresh the UI of their own board
    *  Enemy players use m_updateEnemyTile to update the view of their boards in the UI
    */
    void slot_updateBoard(quint8 ownSlotID);
    void slot_setUpGame(quint8 bSlotID);

private:
    // UI
    Ui::Match *ui;
    QWidget *gameContainer;
    QGridLayout *gridLayout;

    // Game state
    std::shared_ptr<GameState> m_pGameState;
    quint8 m_bSlotID;

    VOF::Action currentAction = VOF::Click;

    quint8 m_level = 1;
    quint8 m_currentScore = 0;
    quint8 m_totalScore = 0;

    // Memo buttons
    QButtonGroup *memoGroup;
    void m_setUpMemoButtons();
    void m_setMemoButtonsVisible(bool fVisible);

    // Replay
    QString m_replayFile;
    int m_gameId = 1;

    // Win/Lose handling
    QTimer* m_winTimer = nullptr;
    int m_winCountdown = 0;

    // Helpers
    /** @brief Board UI setup
    *
    *  Creates the 5x5 Grid in the UI
    */
    void m_setUpBoard();
    void m_resetBoard();
    /** @brief Set up the UI boards for enemy players
    *
    *  Creates the boards for up to 8 enemy players
    *  Each enemy board is a Widget with a 5x5 grid of Label tiles
    *  The color are set according to the player's assigned color
    */
    void m_setUpEnemyBoards();
    void m_resetEnemyBoards();
    /** @brief Reveal Tile upon clicking
    *
    *  Reveals the value of the tile the player has clicked on
    */
    void m_handleCardClick(CardButton *btn);

    void m_updateWidgets();
    /** @brief Update a single tile on the enemy players board
    *
    *  Finds the tile at the given row and column for the enemy player and updates based on the value
    *  If the tile is a bomb, displays the whole with a red background
    *  otherwise green for value 1, blue for value 2 and pruple for value 3
    */
    void m_updateEnemyTile(quint8 player, quint8 row, quint8 col, VOF::Tile tile);
    /** @brief Update a single tile on the local players board
    *
    *  Finds the tile at the given row and column for the local player and updates based on the value
    *  If the tile is a bomb, displays "X" with a red background, otherwise show the value
    *  Also disables the button after its tile has been revealed
    *  Removes memo tag on the tile
    */
    void m_updatePlayerTile(quint8 row, quint8 col);

    QString buildFieldState() const;
    /** @brief Update all player progress bars to reflect current scores
    *
    *  Goes through all players in the game state and updates the corresponding Progress bar for each player based on their total score
    */
    void m_updateProgressBars();
    void m_initializeProgressBars();
};

#endif // MATCH_H
