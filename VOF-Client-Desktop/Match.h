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
    void backToMenu();        // NEU

private slots:
    void on_quitBtn_clicked(); // NEU

private:
    Ui::Match *ui;
};

#endif // MATCH_H
