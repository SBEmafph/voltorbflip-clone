#include "Lobby.h"
#include "./ui_lobby.h"
#include <QShowEvent>

Lobby::Lobby(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Lobby)
    , matchTimer(new QTimer(this))
    , secondsLeft(10)
{
    ui->setupUi(this);

    matchTimer->setInterval(1000);
    connect(matchTimer, &QTimer::timeout, this, [this]() {
        secondsLeft--;
        ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");

        if (secondsLeft <= 0) {
            matchTimer->stop();
            onMatchTimeout();
        }
    });

    // Replay-Button explizit verbinden (falls Auto-Connect nicht greift)
    connect(ui->replayBtn, &QPushButton::clicked,
            this, &Lobby::on_replayBtn_clicked);
}

void Lobby::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    matchTimer->start();
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::on_quitBtn_clicked()
{
    matchTimer->stop();
    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    emit backToMenu();
    this->hide();
}

void Lobby::onMatchTimeout()
{
    emit startMatch();
    this->hide();
}

// NEU: gleiches Verhalten wie im Menu
void Lobby::on_replayBtn_clicked()
{
    emit backToMenu();   // optional: falls du Menü zeigen willst
    this->hide();        // Lobby schließen
}
