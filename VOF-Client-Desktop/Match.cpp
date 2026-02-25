#include "Match.h"
#include "ui_Match.h"

Match::Match(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Match)
    , memoGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    m_setUpMemoButtons();

    connect(ui->quitBtn, &QPushButton::clicked,
            this, &Match::on_quitBtn_clicked);

    // Create 5x5 Matchboard
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            CardButton *btn = new CardButton(row, col, this);
            btn->setStyleSheet("background-color: #2D7D46; border: 2px solid #E3B448;");
            ui->gameGrid->addWidget(btn, row, col);

            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                handleCardClick(btn);
            });
        }
    }

    resetBoard();
}

Match::~Match()
{
    delete ui;
}

void Match::on_quitBtn_clicked()
{
    resetBoard();
    emit sig_backToMenu();
    this->hide();
}

void Match::m_openMemoButtons()
{
    m_setMemoButtonsVisible(true);
}

void Match::m_closeMemoButtons()
{
    m_setMemoButtonsVisible(false);
}

void Match::m_setUpMemoButtons()
{
    ui->Memo1Btn->setCheckable(true);
    ui->Memo2Btn->setCheckable(true);
    ui->Memo3Btn->setCheckable(true);
    ui->MemoOrbBtn->setCheckable(true);

    memoGroup->addButton(ui->Memo1Btn, VOF::Note1);
    memoGroup->addButton(ui->Memo2Btn, VOF::Note2);
    memoGroup->addButton(ui->Memo3Btn, VOF::Note3);
    memoGroup->addButton(ui->MemoOrbBtn, VOF::NoteBomb);
    memoGroup->setExclusive(true);

    connect(memoGroup, &QButtonGroup::idClicked, this,
            [this](int id){
                VOF::Action clickedAction = static_cast<VOF::Action>(id);

                if (currentAction == clickedAction) {
                    currentAction = VOF::Click;
                    memoGroup->setExclusive(false);
                    if (memoGroup->checkedButton())
                        memoGroup->checkedButton()->setChecked(false);
                    memoGroup->setExclusive(true);
                } else {
                    currentAction = clickedAction;
                }
            });

    m_setMemoButtonsVisible(false);

    connect(ui->OpenMemoBtn,  &QPushButton::clicked,
            this, &Match::m_openMemoButtons);
    connect(ui->CloseMemoBtn, &QPushButton::clicked,
            this, &Match::m_closeMemoButtons);
}

void Match::m_setMemoButtonsVisible(bool fVisible)
{
    ui->Memo1Btn->setVisible(fVisible);
    ui->Memo2Btn->setVisible(fVisible);
    ui->Memo3Btn->setVisible(fVisible);
    ui->MemoOrbBtn->setVisible(fVisible);
}

void Match::handleCardClick(CardButton *btn)
{
    if (currentAction == VOF::Click) {
        btn->setText("2"); // Example
        btn->setStyleSheet("background-color: #4CAF50; color: black;");
        btn->setEnabled(false);

        // Clear any memo icons once the card is revealed
        for (int i = 0; i < 4; ++i)
            btn->memos[i]->hide();
    }
    else {
        btn->toggleMemo(currentAction);
    }

    // Forward user action to game/server logic
    emit sig_action(currentAction, btn->c, btn->r);
}

void Match::resetBoard()
{
    currentAction = VOF::Click;

    memoGroup->setExclusive(false);
    if (memoGroup->checkedButton())
        memoGroup->checkedButton()->setChecked(false);
    memoGroup->setExclusive(true);

    for (int row = 0; row < ui->gameGrid->rowCount(); ++row) {
        for (int col = 0; col < ui->gameGrid->columnCount(); ++col) {
            QLayoutItem *item = ui->gameGrid->itemAtPosition(row, col);
            if (!item) continue;

            CardButton *btn = qobject_cast<CardButton*>(item->widget());
            if (!btn) continue;

            btn->setText("");
            btn->setEnabled(true);
            btn->setStyleSheet("background-color: #2D7D46; border: 2px solid #E3B448;");

            for (int i = 0; i < 4; ++i)
                btn->memos[i]->hide();
        }
    }
}
