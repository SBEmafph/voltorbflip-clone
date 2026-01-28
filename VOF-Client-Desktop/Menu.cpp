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
    , BrowserWindow(new Browser(this))
{
    ui->setupUi(this);

    rulesWindow->hide();
    replayWindow->hide();
    shopWindow->hide();
    SettingsWindow->hide();
    BrowserWindow->hide();
    lobby->hide();

    // Browser Form
    connect(BrowserWindow, &Browser::connectRequested,
            this, &Menu::on_BrowserConnect);

    // Lobby → zurück zum Menu
    connect(lobby, &Lobby::backToMenu, this, [this]() {
        this->show();
    });

    // Lobby → Match
    connect(lobby, &Lobby::startMatch, this, [this]() {
        lobby->hide();
        matchWindow->show();
    });

    // Match Form
    connect(matchWindow, &Match::backToMenu, this, [this]() {
        this->show();
    });

    connect(ui->ReplayBtn, &QPushButton::clicked, this, &Menu::on_ReplayBtn_clicked);
    connect(ui->ShopBtn, &QPushButton::clicked, this, &Menu::on_ShopBtn_clicked);
    connect(ui->SettingsBtn, &QPushButton::clicked, this, &Menu::on_SettingsBtn_clicked);
    connect(ui->PlayBtn, &QPushButton::clicked, this, &Menu::on_PlayBtn_clicked);
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
    delete BrowserWindow;
}

void Menu::on_PlayBtn_clicked()
{
    BrowserWindow->show();
}

void Menu::on_BrowserConnect()
{
    BrowserWindow->hide();
    this->hide();
    lobby->show();
}

void Menu::on_RulesBtn_clicked()
{
    rulesWindow->show();
}

void Menu::on_SettingsBtn_clicked()
{
    SettingsWindow->show();
}

void Menu::on_ReplayBtn_clicked()
{
    replayWindow->show();
}

void Menu::on_ShopBtn_clicked()
{
    shopWindow->show();
}
