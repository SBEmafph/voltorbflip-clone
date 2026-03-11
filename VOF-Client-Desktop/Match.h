#ifndef MATCH_H
#define MATCH_H

#include <QWidget>
#include <QButtonGroup>
#include <QGridLayout>
#include <QTimer>

#include "Replay.h"
#include "CardButton.h"
#include "GlobalDefines.h"
#include "Gamelogic.h"

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
    void resetBoard();
    void startLevel();
    void updateRowColLabels( const QVector<quint8>& RowSums, const QVector<quint8>& ColSums, const QVector<quint8>& RowMines, const QVector<quint8>& ColMines);

signals:
    void sig_backToMenu();
    void sig_action(VOF::Action action, quint8 x, quint8 y);

private slots:
    void on_quitBtn_clicked();
    void m_openMemoButtons();
    void m_closeMemoButtons();
    void openReplay(int replayId);

private:
    // UI
    Ui::Match *ui;
    QWidget *gameContainer;
    QGridLayout *gridLayout;

    // Game state
    VOF::Action currentAction = VOF::Click;
    Field m_field;            // 5x5 game field (25 values)
    bool  m_revealed[25];     // revealed fields
    quint8 m_level = 1;
    quint8 m_currentScore = 0;
    quint8 m_totalScore = 0;
    bool m_levelCompleted = false;

    // Memo buttons
    QButtonGroup *memoGroup;
    void m_setUpMemoButtons();
    void m_setMemoButtonsVisible(bool fVisible);

    // Replay
    QString buildReplayPath(int replayId) const;
    QString m_replayFile;
    int m_gameId = 1;
    replay* replayWindow;
    int findNextFreeReplayId() const;

    // Win/Lose handling
    QTimer* m_winTimer = nullptr;
    int m_winCountdown = 0;

    // Helpers
    QString buildFieldState() const;
    void handleCardClick(CardButton *btn);
    void updateWidgets();
};

#endif // MATCH_H
