#include "Lobby.h"
#include "./ui_Lobby.h"
#include <QShowEvent>

Lobby::Lobby(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Lobby)
    , matchTimer(new QTimer(this))
    , secondsLeft(10) // countdown in seconds
{
    ui->setupUi(this);

    // Connect the timeout to decrease secondsLeft
    // matchTimer->setInterval(1000); // 1 second
    // connect(matchTimer, &QTimer::timeout, this, [this]() {
    //     secondsLeft--;
    //     ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");

    //     if (secondsLeft <= 0) {
    //         matchTimer->stop();
    //         onMatchTimeout();  // reuse your existing function
    //     }
    // });
}

// Reset timer every time the Lobby window is shown
void Lobby::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // Reset countdown
    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    matchTimer->start();
}

Lobby::~Lobby()
{
    delete ui;
}

// Quit button clicked
void Lobby::on_quitBtn_clicked()
{
    matchTimer->stop();
    secondsLeft = 10;  // reset countdown
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    emit backToMenu();
    this->hide();
}

// Called when 10 seconds are up
void Lobby::onMatchTimeout()
{
    emit startMatch();
    this->hide();
}
