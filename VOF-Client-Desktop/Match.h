#ifndef MATCH_H
#define MATCH_H

#include <QMainWindow>

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
    void on_quitBtn_clicked();   // BESTEHEND

    // >>> NEU (nach deinen Regeln)
    void VerbOpenMemoButtons();
    void VerbCloseMemoButtons();

private:
    Ui::Match *ui;   // BESTEHEND

    // >>> NEU
    void VerbSetMemoButtonsVisible(bool fVisible);
};

#endif // MATCH_H
