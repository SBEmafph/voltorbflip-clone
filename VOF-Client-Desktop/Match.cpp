#include "Match.h"
#include "ui_Match.h"
#include "GameLogic.h"

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

    m_gameId = 1;
    m_replayFile = buildReplayPath(m_gameId);

    GameLogic::ResetReplayFile(m_replayFile);
    GameLogic::ReplayStartGame(m_replayFile, m_gameId);

    m_initializeProgressBars();  

    m_setUpMemoButtons();

    connect(ui->quitBtn, &QPushButton::clicked,
            this, &Match::on_quitBtn_clicked);

    m_setUpBoard();

    m_setUpEnemyBoards();

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
    m_startLevel();
    m_resetBoard();
    m_resetEnemyBoards();

    emit sig_backToMenu();
    this->hide();
}

void Match::on_openMemoButtons()
{
    m_setMemoButtonsVisible(true);
}

void Match::on_closeMemoButtons()
{
    m_setMemoButtonsVisible(false);
}

void Match::slot_updateBoard(quint8 ownSlotID)
{
    for (auto it = m_pGameState->tPlayerList.begin(); it != m_pGameState->tPlayerList.end(); ++it)
    {
        quint8 playerId = it.key();
        const PlayerSessionState& player = it.value();

        for (int i = 0; i < 25; ++i)
        {
            if (player.fRevealed[i])
            {
                int row = i / 5;
                int col = i % 5;
                VOF::Tile tileValue = static_cast<VOF::Tile>(player.bBoard[i]);

                if (playerId == ownSlotID) {
                    m_updatePlayerTile(row, col);
                }

                m_updateEnemyTile(playerId, row, col, tileValue);
            }
        }
    }
}

void Match::slot_setUpGame(quint8 bSlotID)
{
    m_bSlotID = bSlotID;
    m_startLevel();
}

void Match::m_setUpMemoButtons()
{
    ui->Memo1Btn->setCheckable(true);
    ui->Memo2Btn->setCheckable(true);
    ui->Memo3Btn->setCheckable(true);
    ui->Memo0OrbBtn->setCheckable(true);

    memoGroup->addButton(ui->Memo1Btn, VOF::Note1);
    memoGroup->addButton(ui->Memo2Btn, VOF::Note2);
    memoGroup->addButton(ui->Memo3Btn, VOF::Note3);
    memoGroup->addButton(ui->Memo0OrbBtn, VOF::NoteBomb);
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
            this, &Match::on_openMemoButtons);
    connect(ui->CloseMemoBtn, &QPushButton::clicked,
            this, &Match::on_closeMemoButtons);
}

void Match::m_setMemoButtonsVisible(bool fVisible)
{
    ui->Memo1Btn->setVisible(fVisible);
    ui->Memo2Btn->setVisible(fVisible);
    ui->Memo3Btn->setVisible(fVisible);
    ui->Memo0OrbBtn->setVisible(fVisible);
}

void Match::m_setUpBoard()
{
    // Create 5x5 Matchboard
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 5; ++col) {
            CardButton *btn = new CardButton(row, col, this);
            btn->setObjectName(QString("player_client_Tile_%1_%2").arg(row).arg(col));
            btn->setStyleSheet("background-color: #2D7D46; border: 2px solid #E3B448;");
            ui->gameGrid->addWidget(btn, row, col);

            connect(btn, &QPushButton::clicked, this, [this, btn]() {
                m_handleCardClick(btn);
            });
        }
    }
}

void Match::m_setUpEnemyBoards()
{
    //field showing other players
    for(int player = 0; player < 8; player++) {
        QWidget *enemyWidget = new QWidget(this);
        enemyWidget->setObjectName(QString("enemy_%1").arg(player));
        int playerRow = player % 4;
        int playerCol = player / 4;

        enemyWidget->setStyleSheet(QString(
                                       ".QWidget { "
                                       "   background-color: #309F6A; "
                                       "   border: 1px solid %1; "
                                       "}"
                                       ).arg(VOF::playerColors[player]));

        QGridLayout *enemylayout = new QGridLayout(enemyWidget);
        enemylayout->setContentsMargins(1,1,1,1);
        enemylayout->setSpacing(0);
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 5; ++col) {
                QLabel *enemyField = new QLabel(" ", enemyWidget);
                enemyField->setObjectName(QString("player_%1_Tile_%2_%3").arg(player).arg(row).arg(col));
                enemyField->setAlignment(Qt::AlignCenter);
                enemylayout->addWidget(enemyField, row, col);
            }
        }
        ui->enemyGrid->addWidget(enemyWidget, playerRow, playerCol);
    }
}

void Match::m_resetEnemyBoards(){
    for(int player = 1; player < 8; player++) {
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 5; ++col) {
                QString searchName = QString("player_%1_Tile_%2_%3").arg(player).arg(row).arg(col);
                QLabel *foundTile = this->findChild<QLabel *>(searchName);
                if (foundTile) {
                    foundTile->setStyleSheet(QString("background-color: %1").arg("309F6A"));
                }
            }
        }
    }
}

void Match::m_handleCardClick(CardButton *btn)
{
    const int idx = btn->r * 5 + btn->c;

    if (currentAction != VOF::Click) {
        btn->toggleMemo(currentAction);
        emit sig_action(currentAction, btn->c, btn->r);
        return;
    }

    if (m_pGameState->tPlayerList[m_bSlotID].fRevealed[idx])
        return;

    /*
    GameLogic::RevealTileWithScore(
        m_pGameState->tPlayerList[m_bSlotID].bBoard,
        m_pGameState->tPlayerList[m_bSlotID].fRevealed,
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


    if (GameLogic::FinishLevelIfCompleted(
            m_pGameState->tPlayerList[m_bSlotID].bBoard,
            m_pGameState->tPlayerList[m_bSlotID].fRevealed,
            m_currentScore,
            m_totalScore,
            m_level))
    {
        m_updateWidgets();

        //Check for win after completing a level
        if (m_totalScore >= 10)
        {
             m_updateWidgets();

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
                     m_startLevel();

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

        m_startLevel();
        return;
    }

    m_updateWidgets();

    quint8 tileValue = m_pGameState->tPlayerList[m_bSlotID].bBoard[idx];
    if(tileValue == 0) tileValue = 4;
    m_updateTile(1, btn->r, btn->c, static_cast<VOF::Tile>(tileValue));
    */

    emit sig_action(currentAction, btn->c, btn->r);
}

void Match::m_initializeProgressBars(){
    for(int i = 1; i < VOF::MAX_CLIENTS; i++){
        QString searchName = QString("score_P%1").arg(i);
        QProgressBar *foundTile = this->findChild<QProgressBar *>(searchName);
        if(foundTile){
            foundTile->setRange(0, 100);
            foundTile->setValue(100);
        }
    }
}

void Match::m_updateProgressBars(){
    for (auto it = m_pGameState->tPlayerList.begin(); it != m_pGameState->tPlayerList.end(); ++it)
    {
        quint8 playerId = it.key();
        const PlayerSessionState& player = it.value();

        QString searchName = QString("score_P%1").arg(playerId+1);
        QProgressBar *foundTile = this->findChild<QProgressBar *>(searchName);
        if(foundTile){
            foundTile->setValue(player.bTotalScore); // Scorebar
        }
    }
}

void Match::m_updateWidgets()
{
    const PlayerSessionState& playerClient = m_pGameState->tPlayerList[m_bSlotID];
    ui->CurrentScoreLabel->setText(QString("Current: %1").arg(playerClient.bCurrentScore)); // Current Score
    ui->TotalScoreLabel->setText(QString("Total: %1").arg(playerClient.bTotalScore)); // Total Score
    ui->LevelLabel->setText(QString("Level: %1").arg(playerClient.bLevel)); // Current Level

    m_updateProgressBars();
}

void Match::m_updateEnemyTile(quint8 player, quint8 row, quint8 col, VOF::Tile tile)
{
    QString searchName = QString("player_%1_Tile_%2_%3").arg(player).arg(row).arg(col);
    QLabel *foundTile = this->findChild<QLabel *>(searchName);
    //LOG_OUT << searchName << " tile *" << foundTile << Qt::endl;
    if (foundTile) {
        foundTile->setStyleSheet(QString("background-color: %1").arg(VOF::tileColors[tile-1]));
    }
}

void Match::m_updatePlayerTile(quint8 row, quint8 col)
{
    QString searchName = QString("player_client_Tile_%1_%2").arg(row).arg(col);
    CardButton *btn = this->findChild<CardButton *>(searchName);
    //LOG_OUT << searchName << " tile *" << btn << Qt::endl;
    if (btn) {
        const int idx = btn->r * 5 + btn->c;
        quint8 value = m_pGameState->tPlayerList[m_bSlotID].bBoard[idx];

        if (value == VOF::Tile::Bomb) {
            btn->setText("X");
            btn->setStyleSheet("background-color: #B71C1C; color: white;");
        } else {
            btn->setText(QString::number(value));
            btn->setStyleSheet("background-color: #4CAF50; color: black;");
        }

        btn->setEnabled(false);

        for (int i = 0; i < 4; ++i)
            btn->memos[i]->hide();

        m_updateWidgets();
    }
}

void Match::m_startLevel()
{
    m_currentScore = 0;
    m_resetBoard();
    m_updateWidgets();
}

void Match::m_updateRowColLabels(
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

void Match::m_resetBoard()
{
    currentAction = VOF::Click;
    m_currentScore = 0;

    QVector<quint8> rowSums, colSums, rowMines, colMines;
    GameLogic::CalculateSumsAndMines(
        m_pGameState->tPlayerList[m_bSlotID].bBoard,
        rowSums,
        colSums,
        rowMines,
        colMines
        );
    m_updateRowColLabels(rowSums, colSums, rowMines, colMines);

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
}

QString Match::buildFieldState() const
{
    QString state;

    for (int i = 0; i < 25; ++i)
    {
        if (i > 0)
            state += ";";

        state += QString::number(m_pGameState->tPlayerList[m_bSlotID].bBoard[i]);
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

