#include "Menu.h"
#include "./ui_Menu.h"

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
    , lobby(new Lobby(this))
    , rulesWindow(new rules(this))
    , matchWindow(new Match(this))
    , replayWindow(new replay(this))
    , shopWindow(new shop(this))
    , SettingsWindow(new Settings(this))
{
    ui->setupUi(this);
    rulesWindow->hide();
    replayWindow->hide();
    shopWindow->hide();
    SettingsWindow->hide();

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

    // Replay öffnen
    connect(ui->ReplayBtn, &QPushButton::clicked,
            this, &Menu::on_ReplayBtn_clicked);

    // Shop öffnen
    connect(ui->ShopBtn, &QPushButton::clicked,
            this, &Menu::on_ShopBtn_clicked);

    connect(ui->SettingsBtn, &QPushButton::clicked,
            this, &Menu::on_SettingsBtn_clicked);
}

Menu::~Menu()
{
    delete ui;
    delete lobby;
    delete rulesWindow;
    delete matchWindow;
    delete replayWindow;
    delete shopWindow;
    delete SettingsWindow;
}

void Menu::on_PlayBtn_clicked()
{
    lobby->show();
    this->hide();
}

void Menu::on_RulesBtn_clicked()
{
    rulesWindow->show();
}

void Menu::on_ReplayBtn_clicked()
{
    replayWindow->show();
}

void Menu::on_ShopBtn_clicked()
{
    shopWindow->show();
}

void Menu::on_SettingsBtn_clicked()
{
    SettingsWindow->show();
}
