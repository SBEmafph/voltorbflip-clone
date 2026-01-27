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
}

void Menu::m_setPlayerConfig(quint32 ID, quint16 token, QString name, bool force)
{
    m_client->m_setID(ID);
    m_client->m_setToken(token);
    m_client->m_setName(name);
    m_client->m_updateConfig(force);
}

void Menu::closeEvent(QCloseEvent *event) { //slow close because of connection?
    if (m_client) {
        m_client->slot_detach();
    }
    event->accept();
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_playBtn_clicked()
{
    m_client->slot_attach();
    lobby->show();
    this->hide();
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
