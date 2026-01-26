#include "menu.h"
#include "./ui_menu.h"

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
    , lobby(new Lobby(this))
    , rulesWindow(new rules(this))
    , matchWindow(new Match(this))
    , replayWindow(new replay(this))
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

Menu::~Menu()
{
    delete ui;
    delete lobby;
    delete rulesWindow;
    delete matchWindow;
    delete replayWindow;
}

void Menu::on_playBtn_clicked()
{
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
    this->hide();
}
