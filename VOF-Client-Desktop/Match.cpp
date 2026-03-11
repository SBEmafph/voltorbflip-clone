#include "Match.h"
#include "ui_Match.h"

#include <QTimer>
#include <QCoreApplication>
#include <QDir>

Match::Match(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Match)
    , memoGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    m_gameId = 1;
    m_replayFile = buildReplayPath(m_gameId);

    GameLogic::ResetReplayFile(m_replayFile);
    GameLogic::ReplayStartGame(m_replayFile, m_gameId);

    ui->score_P1->setRange(0, 100);
    ui->score_P1->setValue(0);

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

    m_level = 1;
    m_totalScore = 0;

    startLevel();

    replayWindow = new replay(this);

    connect(replayWindow, &replay::replayImported, this, [this](int importedId){

        int nextId = findNextFreeReplayId();

        if (nextId == -1)
            nextId = 1;

        m_gameId = nextId;
        m_replayFile = buildReplayPath(m_gameId);

        GameLogic::ResetReplayFile(m_replayFile);
        GameLogic::ReplayStartGame(m_replayFile, m_gameId);
    });
}

Match::~Match()
{
    delete ui;
}

void Match::on_quitBtn_clicked()
{
    m_level = 1;
    m_totalScore = 0;
    startLevel();

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
    const int idx = btn->r * 5 + btn->c;

    if (currentAction != VOF::Click) {
        btn->toggleMemo(currentAction);
        emit sig_action(currentAction, btn->c, btn->r);
        return;
    }

    if (m_revealed[idx])
        return;

    GameLogic::RevealTileWithScore(
        m_field,
        m_revealed,
        btn->r,
        btn->c,
        m_currentScore,
        m_level
        );

    GameLogic::ReplayLogAction(
        m_replayFile,
        m_gameId,
        static_cast<int>(currentAction), // 0-Reveal Tile, 1-Set Memo, 2-Item Used, 3-Finished Level
        m_level,
        m_currentScore,
        m_totalScore,
        btn->r,
        btn->c
        );

    quint8 value = m_field[idx];

    if (value == VOF::TILE_MINE) {
        btn->setText("X");
        btn->setStyleSheet("background-color: #B71C1C; color: white;");
    } else {
        btn->setText(QString::number(value));
        btn->setStyleSheet("background-color: #4CAF50; color: black;");
    }

    btn->setEnabled(false);

    for (int i = 0; i < 4; ++i)
        btn->memos[i]->hide();

    if (GameLogic::FinishLevelIfCompleted(
            m_field,
            m_revealed,
            m_currentScore,
            m_totalScore,
            m_level))
    {

         updateWidgets();

        //Check for win after completing a level
        if (m_totalScore >= 10)
        {
             updateWidgets();

             ui->LevelLabel->setText("Gewonnen!");

             replayWindow->setGameResult(true);

             // Block Board Inputs
             for (int row = 0; row < 5; ++row) {
                 for (int col = 0; col < 5; ++col) {
                     if (auto *item = ui->gameGrid->itemAtPosition(row, col)) {
                         if (auto *btn = qobject_cast<CardButton*>(item->widget())) {
                             btn->setEnabled(false);
                         }
                     }
                 }
             }

             m_winCountdown = 5;
             ui->CurrentScoreLabel->setText(
                 QString("Zurück in %1...").arg(m_winCountdown)
                 );

             m_winTimer = new QTimer(this);
             m_winTimer->setInterval(1000);

             connect(m_winTimer, &QTimer::timeout, this, [this]() {
                 m_winCountdown--;

                 if (m_winCountdown > 0) {
                     ui->CurrentScoreLabel->setText(
                         QString("Zurück zum Menü in %1s").arg(m_winCountdown)
                         );
                 } else {
                     m_winTimer->stop();
                     m_winTimer->deleteLater();
                     m_winTimer = nullptr;

                     m_level = 1;
                     m_totalScore = 0;
                     startLevel();

                     GameLogic::ReplayEndGame(
                         m_replayFile,
                         m_gameId,
                         m_level,
                         m_totalScore
                         );

                     int nextId = findNextFreeReplayId();
                     if (nextId != -1)
                     {
                         m_gameId = nextId;
                         m_replayFile = buildReplayPath(m_gameId);
                     }

                     emit sig_backToMenu();
                     this->hide();
                 }
             });

             m_winTimer->start();
             return;
        }

        startLevel();
        return;
    }

    updateWidgets();

    emit sig_action(currentAction, btn->c, btn->r);
}

void Match::updateWidgets()
{
    ui->CurrentScoreLabel->setText(QString("Current: %1").arg(m_currentScore)); // Current Score
    ui->TotalScoreLabel->setText(QString("Total: %1").arg(m_totalScore)); // Total Score
    ui->LevelLabel->setText(QString("Level: %1").arg(m_level)); // Current Level
    ui->score_P1->setValue(m_totalScore); // Scorebar
}

void Match::startLevel()
{
    m_levelCompleted = false;

    m_field = GameLogic::GenerateField5x5_Level(m_level);
    std::fill(std::begin(m_revealed), std::end(m_revealed), false);

    m_currentScore = 0;
    resetBoard();
    updateWidgets();
}

void Match::updateRowColLabels(
    const QVector<quint8>& RowSums,
    const QVector<quint8>& ColSums,
    const QVector<quint8>& RowMines,
    const QVector<quint8>& ColMines)
{
    // Spalten
    for (int i = 0; i < 5; ++i) {
        QLabel* scoreLabel = findChild<QLabel*>(QString("Px_%1").arg(i+1));
        if (scoreLabel)
            scoreLabel->setText(QString::number(ColSums[i]));

        QLabel* mineLabel = findChild<QLabel*>(QString("Mx_%1").arg(i+1));
        if (mineLabel)
            mineLabel->setText(QString::number(ColMines[i]));
    }

    // Reihen
    for (int i = 0; i < 5; ++i) {
        QLabel* scoreLabel = findChild<QLabel*>(QString("P%1_x").arg(i+1));
        if (scoreLabel)
            scoreLabel->setText(QString::number(RowSums[i]));

        QLabel* mineLabel = findChild<QLabel*>(QString("M%1_x").arg(i+1));
        if (mineLabel)
            mineLabel->setText(QString::number(RowMines[i]));
    }
}

void Match::resetBoard()
{
    currentAction = VOF::Click;

    m_currentScore = 0;

    QVector<quint8> rowSums, colSums, rowMines, colMines;
    GameLogic::CalculateSumsAndMines(m_field, rowSums, colSums, rowMines, colMines);
    updateRowColLabels(rowSums, colSums, rowMines, colMines);

    memoGroup->setExclusive(false);
    if (memoGroup->checkedButton())
        memoGroup->checkedButton()->setChecked(false);
    memoGroup->setExclusive(true);

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            if (auto *item = ui->gameGrid->itemAtPosition(row, col)) {
                if (auto *btn = qobject_cast<CardButton*>(item->widget())) {
                    btn->setText("");
                    btn->setEnabled(true);
                    btn->setStyleSheet("background-color: #2D7D46; border: 2px solid #E3B448;");
                    for (int i = 0; i < 4; ++i)
                        btn->memos[i]->hide();
                }
            }
        }
    }

     updateWidgets();
}

QString Match::buildFieldState() const
{
    QString state;

    for (int i = 0; i < 25; ++i)
    {
        if (i > 0)
            state += ";";

        state += QString::number(m_field[i]);
    }

    return state;
}

QString Match::buildReplayPath(int replayId) const
{
    QString basePath = QCoreApplication::applicationDirPath() + "/Replays/";
    QDir().mkpath(basePath); // Check if Folder even exists

    return basePath + QString("replay%1.csv").arg(replayId);
}

void Match::openReplay(int replayId)
{
    m_gameId = replayId;
    m_replayFile = buildReplayPath(replayId);

    GameLogic::ResetReplayFile(m_replayFile);
    GameLogic::ReplayStartGame(m_replayFile, m_gameId);
}

int Match::findNextFreeReplayId() const
{
    QString basePath = QCoreApplication::applicationDirPath() + "/Replays/";

    for (int i = 1; i <= 5; ++i) {
        QString filePath = basePath + QString("replay%1.csv").arg(i);

        QFile file(filePath);
        bool isFree = true;

        if (file.exists()) {
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                int lineCount = 0;
                while (!file.atEnd()) {
                    QByteArray line = file.readLine().trimmed();
                    if (!line.isEmpty())
                        lineCount++;
                }
                file.close();
                if (lineCount > 1)
                    isFree = false;
            }
        }

        if (isFree)
            return i;
    }

    return -1; // Keine freie Datei
}

