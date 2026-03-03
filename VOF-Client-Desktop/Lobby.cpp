#include "Lobby.h"
#include "./ui_Lobby.h"
#include <QShowEvent>

Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Lobby)
    , matchTimer(new QTimer(this))
    , secondsLeft(10)
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Fenstergröße anpassen blockieren
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
    connect(ui->ReplayBtn, &QPushButton::clicked,
            this, &Lobby::on_ReplayBtn_clicked);
}

void Lobby::showEvent(QShowEvent *event)
{
    /*
    QMainWindow::showEvent(event);

    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    */
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::on_StartBtn_clicked()
{
    if (matchTimer->isActive())
        return;

    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    matchTimer->start();
}

void Lobby::on_quitBtn_clicked()
{
    matchTimer->stop();
    secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(secondsLeft) + "s");
    emit backToMenu();
    //this->hide();
}

void Lobby::onMatchTimeout()
{
    emit startMatch();
    //this->hide();
}

void Lobby::on_ReplayBtn_clicked()
{
    //this->show();
}

void Lobby::on_ShopBtn_clicked()
{
    //this->show();
}
