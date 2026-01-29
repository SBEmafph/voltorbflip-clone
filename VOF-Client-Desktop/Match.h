#ifndef MATCH_H
#define MATCH_H

#include <QMainWindow>
#include <QButtonGroup>

#include "CardButton.h"
#include "GlobalDefines.h"

namespace Ui {
class Match;
}

class Match : public QMainWindow
{
    Q_OBJECT

public:
    explicit Match(QWidget *parent = nullptr);
    ~Match();

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
    VOF::Action currentAction = VOF::Click; // 0 = Aufdecken, 1-3 = Zahlen-Notiz, 4 = Voltorb-Notiz
    QButtonGroup *memoGroup;

    void m_setUpMemoButtons();
    void m_setMemoButtonsVisible(bool fVisible);
    void handleCardClick(CardButton *btn);
};

#endif // MATCH_H
