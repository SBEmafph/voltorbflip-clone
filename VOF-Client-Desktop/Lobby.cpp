#include "Lobby.h"
#include "./ui_Lobby.h"

#include "GlobalDefines.h"

#include <QShowEvent>
#include <QRandomGenerator>
#include <QStyle>

Lobby::Lobby(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Lobby)
    , m_matchTimer(new QTimer(this))
    , m_secondsLeft(10)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    m_matchTimer->setInterval(1000);

    connect(m_matchTimer, &QTimer::timeout, this, [this]() {
        m_secondsLeft--;
        ui->timerLabel->setText("Game starts in " + QString::number(m_secondsLeft) + "s");
        if (m_secondsLeft <= 0) {
            m_matchTimer->stop();
            slot_onMatchTimeout();
        }
    });

    // Connect replay button explicitly
    connect(ui->replayBtn, &QPushButton::clicked, this, &Lobby::on_replayBtn_clicked);
}

Lobby::~Lobby()
{
    delete ui;
}

void Lobby::on_startBtn_clicked()
{
    if(m_fIsHost){
        emit sig_requestStart();
    }
    else{
        QPushButton *button = ui->startBtn;
        if(m_isReady){
            m_isReady = false;
            button->setText("Ready?");
            button->setProperty("ready", m_isReady);
        }
        else{
            m_isReady = true;
            button->setText("Ready!");
            button->setProperty("ready", m_isReady);
        }
        button->style()->unpolish(button);
        button->style()->polish(button);

        emit sig_isReady();
    }
}

void Lobby::on_quitBtn_clicked()
{
    m_matchTimer->stop();
    m_secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(m_secondsLeft) + "s");
    emit sig_backToMenu();
}

void Lobby::slot_onMatchTimeout()
{
    emit sig_startMatch();
}

void Lobby::slot_startMatch()
{
    if (m_matchTimer->isActive()) {
        m_matchTimer->stop();
        m_secondsLeft = 10;
        emit sig_startMatch();
    }

    m_secondsLeft = 10;
    ui->timerLabel->setText("Game starts in " + QString::number(m_secondsLeft) + "s");
    m_matchTimer->start();
}

void Lobby::on_replayBtn_clicked()
{
    //this->show();
}

void Lobby::on_shopBtn_clicked()
{
    //this->show();
}

void Lobby::slot_setHostState(bool isHost)
{
    m_fIsHost = isHost;
}

QLabel* Lobby::m_getLabelBySlot(quint8 bSlotID)
{
    QString searchName = QString("player%1").arg(bSlotID);
    QLabel *foundTile = this->findChild<QLabel *>(searchName);
    return foundTile;
}

void Lobby::m_updatePlayerReadyStatus(quint8 slotId, bool isReady)
{
    QLabel* label = m_getLabelBySlot(slotId);

    // activate ready for color
    label->setProperty("ready", isReady);

    // reload qt stylesheet
    label->style()->unpolish(label);
    label->style()->polish(label);
}

void Lobby::slot_updateLobby(const QString& name, quint8 bSlotID, bool fIsReady)
{
    m_setPlayerName(bSlotID, name);
    m_updatePlayerReadyStatus(bSlotID, fIsReady);

    if(m_fIsHost){
        ui->startBtn->setText("Start Game");
    }
}

void Lobby::m_setPlayerName(quint8 bSlotID, const QString& name)
{
    QString finalName = name;

    if(finalName.isEmpty())
    {
        int index = QRandomGenerator::global()->bounded(VOF::defaultNames.size());
        finalName = VOF::defaultNames.at(index);
    }

    QLabel* foundTile = m_getLabelBySlot(bSlotID);
    foundTile->setText(finalName);
}
