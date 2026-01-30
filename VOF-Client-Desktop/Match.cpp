#include "Match.h"
#include "ui_Match.h"

Match::Match(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Match)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    // >>> NEU
    VerbSetMemoButtonsVisible(false);

    connect(ui->quitBtn,  &QPushButton::clicked, this, &Match::on_quitBtn_clicked);

    // >>> NEU
    connect(ui->OpenMemoBtn,  &QPushButton::clicked, this, &Match::VerbOpenMemoButtons);
    connect(ui->CloseMemoBtn, &QPushButton::clicked, this, &Match::VerbCloseMemoButtons);
}

Match::~Match()
{
    delete ui;
}

// BESTEHEND
void Match::on_quitBtn_clicked()
{
    emit backToMenu();
    this->hide();
}

// >>> NEU
void Match::VerbOpenMemoButtons()
{
    VerbSetMemoButtonsVisible(true);
}

// >>> NEU
void Match::VerbCloseMemoButtons()
{
    VerbSetMemoButtonsVisible(false);
}

// >>> NEU
void Match::VerbSetMemoButtonsVisible(bool fVisible)
{
    ui->Memo1Btn->setVisible(fVisible);
    ui->Memo2Btn->setVisible(fVisible);
    ui->Memo3Btn->setVisible(fVisible);
    ui->MemoOrbBtn->setVisible(fVisible);
}

void Match::handleCardClick(CardButton *btn) {
    if (currentAction == VOF::Click) {
        // Karte normal umdrehen (Logik für Punkte/Game Over)
        btn->setText("2"); // Beispielwert
        btn->setStyleSheet("background-color: #4CAF50; color: black;");
        btn->setEnabled(false);
        for(int i=1; i<=4; i++) btn->toggleMemo(i); // Hack: Toggle twice logic needed?
    }
    else {
        // Nur eine Notiz anheften/entfernen
        btn->toggleMemo(currentAction);
    }
    emit sig_action(currentAction, btn->r, btn->c);
}

