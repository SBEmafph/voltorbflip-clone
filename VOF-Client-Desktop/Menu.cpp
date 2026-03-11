#include "Menu.h"
#include "./ui_Menu.h"
#include "Settings.h"

#include <QCloseEvent>

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
    , m_client(new Client(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Fenstergröße anpassen blockieren
    ui->stack->addWidget(lobby);        //index 1
    ui->stack->addWidget(matchWindow);  //index 2
    ui->stack->setCurrentWidget(ui->menu);

    // Browser Form
    connect(BrowserWindow, &Browser::connectRequested,
            m_client, &Client::slot_attach);

    connect(m_client, &Client::sig_connected,
            this, &Menu::on_BrowserConnect);

    // Lobby → Match
    connect(lobby, &Lobby::startMatch, this, [this]() {
        ui->stack->setCurrentWidget(matchWindow);
    });

    // Lobby → zurück zum Menu
    connect(lobby, &Lobby::backToMenu,
            this, &Menu::on_backToMenu);


    // Match Form
    connect(matchWindow, &Match::sig_backToMenu,
            this, &Menu::on_backToMenu);

    connect(matchWindow, &Match::sig_action,
            m_client, &Client::slot_sendMatchAction);

    connect(ui->ReplayBtn, &QPushButton::clicked,
            this, &Menu::on_ReplayBtn_clicked);
    connect(ui->ShopBtn, &QPushButton::clicked,
            this, &Menu::on_ShopBtn_clicked);
    connect(ui->SettingsBtn, &QPushButton::clicked,
            this, &Menu::on_SettingsBtn_clicked);
    connect(ui->PlayBtn, &QPushButton::clicked,
            this, &Menu::on_PlayBtn_clicked);
    connect(ui->ExitBtn, &QPushButton::clicked,
            this, &Menu::onExitBtn_clicked);
}

void Menu::m_setPlayerConfig(quint32 ID, quint16 token, QString name, bool force)
{
    m_client->m_updateConfig(ID, token, name, force);

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
    lobby->setPlayerName(SettingsWindow->getPlayerName());
    BrowserWindow->show();
}

void Menu::on_BrowserConnect()
{
    BrowserWindow->hide();
    ui->stack->setCurrentWidget(lobby);
}

void Menu::on_backToMenu()
{
    m_client->slot_detach();
    ui->stack->setCurrentWidget(ui->menu);
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

