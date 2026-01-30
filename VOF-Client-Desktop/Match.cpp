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

    connect(ui->quitBtn,  &QPushButton::clicked, this,
            &Match::on_quitBtn_clicked);

    // Quit-Button verbinden (falls nicht per auto-connect)
    connect(ui->quitBtn, &QPushButton::clicked,
            this, &Match::on_quitBtn_clicked);

    // 1. Gitter für Karten erstellen

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            CardButton *btn = new CardButton(row, col, this);
            btn->setStyleSheet("background-color: #2D7D46; border: 2px solid #E3B448;");

            ui->gameGrid->addWidget(btn, row, col);

            // Klick-Logik
            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                this->handleCardClick(btn);
            });
        }
    }
}

Match::~Match(){
    delete ui;
}

void Match::on_quitBtn_clicked()
{
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
    /*
    connect(memoGroup, &QButtonGroup::idClicked,
        this, [this](quint8 id){
        currentAction = (currentAction == id) ?
            VOF::Click : static_cast<VOF::Action>(id);
    });
*/
    connect(memoGroup, &QButtonGroup::idClicked, this, [this](int id){
        VOF::Action clickedAction = static_cast<VOF::Action>(id);
        LOG_OUT << "buttonID " << id
                << " currentAction " << currentAction
                << " clickedAction " << clickedAction
                << Qt::endl;
        if (currentAction == clickedAction) {
            currentAction = VOF::Click;
            memoGroup->setExclusive(false);
            if(memoGroup->checkedButton()) {
                LOG_OUT << "uncheck" << id << Qt::endl;
                memoGroup->checkedButton()->setChecked(false);
                memoGroup->setExclusive(true);
            }
        } else {
            currentAction = clickedAction;
        }
    });

    m_setMemoButtonsVisible(false);
    connect(ui->OpenMemoBtn,  &QPushButton::clicked,
            this, &Match::m_openMemoButtons);
    connect(ui->CloseMemoBtn, &QPushButton::clicked,
            this, &Match::m_closeMemoButtons);
    /*
    connect(ui->Memo1Btn,  &QPushButton::clicked,
            this, [this](){
                currentAction != VOF::Note1 ?
                    currentAction = VOF::Note1 :
                    currentAction = VOF::Click;
            });
    connect(ui->Memo2Btn,  &QPushButton::clicked,
        this, [this](){ currentAction = (currentAction != VOF::Note2 ? VOF::Note2 : VOF::Click; });
*/
}

void Match::m_setMemoButtonsVisible(bool fVisible)
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
        for(int i=0; i<4; i++) btn->memos[i]->hide(); // Hack: Toggle twice logic needed?
    }
    else {
        // Nur eine Notiz anheften/entfernen
        btn->toggleMemo(currentAction);
    }
    emit sig_action(currentAction, btn->c, btn->r);
}
