#include "Menu.h"
#include "./ui_Menu.h"

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
    , lobby(new Lobby(this))
    , rulesWindow(new rules(this))
    , matchWindow(new Match(this))
    , replayWindow(new replay(this))
    , client(new Client(this))
{
    ui->setupUi(this);

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
}

Menu::~Menu()
{
    delete ui;
}

void Menu::on_playBtn_clicked()
{
    client->m_identify();
    lobby->show();
    this->hide();
}

void Menu::on_rulesBtn_clicked()
{
    rulesWindow->show();
    this->hide();
}

void Menu::on_replayBtn_clicked()
{
    replayWindow->show();
    this->hide();
}
