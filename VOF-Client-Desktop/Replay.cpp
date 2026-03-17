#include "Replay.h"
#include "ui_Replay.h"

#include <QDesktopServices>
#include <QUrl>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QShowEvent>
#include <QFileDialog>
#include <QMessageBox>

replay::replay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::replay)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_ShowWithoutActivating, false);

    resetAllReplays();

    // Connect all replay buttons to the same slot
    for (int i = 1; i <= 10; ++i) {
        QPushButton* btn = findChild<QPushButton*>(QString("replay%1Btn").arg(i));
        if (btn) {
            connect(btn, &QPushButton::clicked, this, &replay::onReplayBtnClicked);
        }
    }
}

static bool hasReplayData(const QString& path)
{
    QFile file(path);

    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    in.readLine(); // Header überspringen

    return !in.atEnd();
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
            btn->setEnabled(hasReplayData(buildReplayPath(i)));
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

void replay::resetAllReplays()
{
    QString basePath = QCoreApplication::applicationDirPath() + "/Replays/";

    QDir().mkpath(basePath); // ORDNER ERSTELLEN

    for (int i = 1; i <= 10; ++i)
    {
        QString filePath = basePath + QString("replay%1.csv").arg(i);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << "gameid,action,date,time,level,currentscore,totalscore,field\n";
        }
    }
}

void replay::on_importBtn_clicked()
{
    // Datei auswählen
    QString importPath = QFileDialog::getOpenFileName(
        this,
        "Import Replay CSV",
        QCoreApplication::applicationDirPath(),
        "CSV files (*.csv)"
        );

    if (importPath.isEmpty())
        return; // Abbruch, keine Datei ausgewählt

    // Nächste freie Replay-Datei finden
    QString basePath = QCoreApplication::applicationDirPath() + "/Replays/";
    int targetId = -1;

    for (int i = 6; i <= 10; ++i) {
        QString filePath = basePath + QString("replay%1.csv").arg(i);

        QFile file(filePath);
        bool isFree = true;

        if (hasReplayData(filePath))
            isFree = false;

        if (isFree) {
            targetId = i;
            break;
        }
    }

    if (targetId == -1) {
        QMessageBox::information(this, "Import", "Keine freie Replay-Datei verfügbar.");
        return;
    }

    // Datei direkt kopieren
    QString targetPath = basePath + QString("replay%1.csv").arg(targetId);
    if (!QFile::copy(importPath, targetPath)) {
        // Falls Datei existiert, vorher überschreiben
        QFile::remove(targetPath);
        if (!QFile::copy(importPath, targetPath)) {
            QMessageBox::warning(this, "Fehler", "Replay-Datei konnte nicht kopiert werden.");
            return;
        }
    }

    emit replayImported(targetId);

    // Buttons aktualisieren
    for (int i = 6; i <= 10; ++i) {
        QPushButton* btn = findChild<QPushButton*>(QString("replay%1Btn").arg(i));
        if (btn) {
            btn->setEnabled(hasReplayData(buildReplayPath(i)));
        }
    }

    QMessageBox::information(this, "Import", QString("Replay erfolgreich auf replay%1.csv importiert.").arg(targetId));
}

void replay::on_exportBtn_clicked()
{
    QString replayDir = QCoreApplication::applicationDirPath() + "/Replays/";

    QDir dir(replayDir);
    if (!dir.exists()) {
        QMessageBox::warning(this, "Export", "Replay-Verzeichnis existiert nicht.");
        return;
    }

    // Explorer öffnen
    QDesktopServices::openUrl(QUrl::fromLocalFile(replayDir));
}

