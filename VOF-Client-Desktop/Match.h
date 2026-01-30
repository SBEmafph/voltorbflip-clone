#ifndef MATCH_H
#define MATCH_H

#include <QMainWindow>
#include <QButtonGroup>

#include "CardButton.h"
#include "GlobalDefines.h"
#include "CardButton.h"

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
    void backToMenu();   // BESTEHEND

private slots:
    void on_quitBtn_clicked();
    void m_openMemoButtons();
    void m_closeMemoButtons();
    void backToMenu();


private:
    Ui::Match *ui;   // BESTEHEND
    int currentMemoType = 0; // 0 = Aufdecken, 1-3 = Zahlen-Notiz, 4 = Voltorb-Notiz

    void m_setUpMemoButtons();
    void m_setMemoButtonsVisible(bool fVisible);
    void handleCardClick(CardButton *btn);
};

#endif // MATCH_H
