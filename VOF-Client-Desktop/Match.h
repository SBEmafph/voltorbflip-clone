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
    void m_startLevel();
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
    void on_quitBtn_clicked();
    void on_openMemoButtons();
    void on_closeMemoButtons();

    void slot_updateBoard(quint8 ownSlotID);
    void slot_setUpGame(quint8 bSlotID);

    void openReplay(int replayId);

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
    QString buildReplayPath(int replayId) const;
    QString m_replayFile;
    int m_gameId = 1;
    replay* replayWindow;

    // Win/Lose handling
    QTimer* m_winTimer = nullptr;
    int m_winCountdown = 0;

    // Helpers
    void m_setUpBoard();
    void m_resetBoard();

    void m_setUpEnemyBoards();
    void m_resetEnemyBoards();

    void m_handleCardClick(CardButton *btn);

    void m_updateWidgets();
    void m_updateEnemyTile(quint8 player, quint8 row, quint8 col, VOF::Tile tile);
    void m_updatePlayerTile(quint8 row, quint8 col);

    QString buildFieldState() const;
    void m_updateProgressBars();
    void m_initializeProgressBars();
};

#endif // MATCH_H
