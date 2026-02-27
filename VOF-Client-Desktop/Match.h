#ifndef MATCH_H
#define MATCH_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QGridLayout>

#include "CardButton.h"
#include "GlobalDefines.h"
#include "Gamelogic.h"

namespace Ui {
class Match;
}

class Match : public QMainWindow
{
    Q_OBJECT

public:
    explicit Match(QWidget *parent = nullptr);
    ~Match();
    void resetBoard();

signals:
    void sig_backToMenu();
    void sig_action(VOF::Action action, quint8 x, quint8 y);

private slots:
    void on_quitBtn_clicked();
    void m_openMemoButtons();
    void m_closeMemoButtons();

private:
    Ui::Match *ui;
    QWidget *gameContainer;
    QGridLayout *gridLayout;

    VOF::Action currentAction = VOF::Click;
    QButtonGroup *memoGroup;

    Field m_field;          // 5x5 Spielfeld (25 Werte)
    bool  m_revealed[25];   // Aufgedeckte Felder
    quint8 m_level = 1;
    quint8 m_currentScore = 0;

    void m_setUpMemoButtons();
    void m_setMemoButtonsVisible(bool fVisible);
    void handleCardClick(CardButton *btn);
};

#endif // MATCH_H
