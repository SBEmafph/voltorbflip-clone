#include "Menu.h"
#include "./ui_Menu.h"

#include <QCloseEvent>

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
    , lobby(new Lobby(this))
    , rulesWindow(new rules(this))
    , matchWindow(new Match(this))
    , replayWindow(new replay(this))
    , m_client(new Client(this))
{
    ui->setupUi(this);
    rulesWindow->hide();

    // Zurück von Lobby
    connect(lobby, &Lobby::backToMenu, this, [this]() {
        this->show();
    });

    // Lobby → Match
    connect(lobby, &Lobby::startMatch, this, [this]() {
        this->hide();
        matchWindow->show();
    });

    // Zurück von Rules
    connect(rulesWindow, &rules::backToMenu, this, [this]() {
        this->show();
    });

    // Zurück von Match
    connect(matchWindow, &Match::backToMenu, this, [this]() {
        this->show();
    });

    // Zurück von Replay
    connect(replayWindow, &replay::backToMenu, this, [this]() {
        this->show();
    });

    // Replay-Button auf menu.ui explizit verbinden (falls Auto-Connect nicht greift)
    connect(ui->replayBtn, &QPushButton::clicked,
            this, &Menu::on_replayBtn_clicked);

    connect(ui->exitBtn, &QPushButton::clicked,
            this, &Menu::onExitBtn_clicked);
}

void Menu::m_setPlayerConfig(quint32 ID, quint16 token, QString name, bool force)
{
    m_client->m_updateConfig(ID, token, name, force);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_playBtn_clicked()
{
    m_client->slot_attach();
    m_client->slot_joinSelectedLobby();
    lobby->show();
    //this->hide();
}

void Menu::on_rulesBtn_clicked()
{
    rulesWindow->show();
    //this->hide();
}

void Menu::on_replayBtn_clicked()
{
    replayWindow->show();
    //this->hide();
}

void Menu::onExitBtn_clicked()
{
    m_client->slot_detach();

    if (m_client->m_getTcpSocket()->state() == QAbstractSocket::UnconnectedState) {
        LOG_OUT << "Socket already closed. Quitting immediately." << Qt::endl;
        qApp->quit();
    } else {
        connect(m_client->m_getTcpSocket(),
        &QTcpSocket::disconnected, qApp, &QCoreApplication::quit);
        QTimer::singleShot(1000, qApp, &QCoreApplication::quit);
    }
}
