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
