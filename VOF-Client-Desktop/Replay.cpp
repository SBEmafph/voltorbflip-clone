#include "Replay.h"
#include "ui_Replay.h"

#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QShowEvent>

static bool replayExists(const QString& filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    int lineCount = 0;
    while (!in.atEnd())
    {
        in.readLine();
        lineCount++;
    }

    file.close();

    return lineCount > 1; // Header + mindestens eine Aktion
}

replay::replay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::replay)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);

    // Connect all replay buttons to the same slot
    for (int i = 1; i <= 10; ++i) {
        QPushButton* btn = findChild<QPushButton*>(QString("replay%1Btn").arg(i));
        if (btn) {
            connect(btn, &QPushButton::clicked, this, &replay::onReplayBtnClicked);
        }
    }
}

replay::~replay()
{
    delete ui;
}

void replay::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    // Enable buttons only if file exists
    for (int i = 1; i <= 10; ++i) {
        QPushButton* btn = findChild<QPushButton*>(QString("replay%1Btn").arg(i));
        if (btn) {
            btn->setEnabled(replayExists(buildReplayPath(i)));
        }
    }
}

void replay::on_exitBtn_clicked()
{
    this->hide();
}

void replay::onReplayBtnClicked()
{
    // Identify which button was clicked
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn)
        return;

    QString name = senderBtn->objectName(); // e.g., "replay3Btn"
    int gameId = name.mid(6, name.length() - 9).toInt(); // extract number from "replayXBtn"

    QString filePath = buildReplayPath(gameId);
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void replay::setGameResult(bool won)
{
    m_gameWon = won;

    if(m_gameWon)
    {
        ui->replay1Btn->setStyleSheet("background-color: #47D45A;");
    }
    else
    {
        ui->replay1Btn->setStyleSheet("background-color: #DE7055;");
    }
}

QString replay::buildReplayPath(int gameId) const
{
    QString basePath = QCoreApplication::applicationDirPath() + "/Replays/";
    return basePath + QString("replay%1.csv").arg(gameId);
}
