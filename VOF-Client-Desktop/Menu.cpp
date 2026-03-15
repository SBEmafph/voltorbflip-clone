#include "Menu.h"
#include "./ui_Menu.h"
#include "Settings.h"
#include "Client.h"

#include <QCloseEvent>

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Menu)
    , lobby(new Lobby(this))
    , rulesWindow(new Rules(this))
    , matchWindow(new Match(this))
    , replayWindow(new replay(this))
    , shopWindow(new shop(this))
    , SettingsWindow(new Settings(this))
    , browserWindow(new Browser(this))
    , m_client(new Client(this))
{
    ui->setupUi(this);
    this->setFixedSize(this->size()); //Fenstergröße anpassen blockieren
    ui->stack->addWidget(lobby);        //index 1
    ui->stack->addWidget(matchWindow);  //index 2
    ui->stack->setCurrentWidget(ui->menu);

    // Browser Form
    connect(browserWindow, &Browser::sig_connectRequested,
            m_client, &Client::on_attach);

    connect(browserWindow, &Browser::sig_disconnect,
            m_client, &Client::on_detach);

    // Client
    connect(m_client, &Client::sig_connected,
            this, &Menu::slot_BrowserConnect);

    connect(m_client, &Client::sig_lobbyUpdate,
            lobby, &Lobby::slot_updateLobby);

    connect(m_client, &Client::sig_setHostState,
            lobby , &Lobby::slot_setHostState);

    connect(lobby, &Lobby::sig_requestStart,
            m_client , &Client::slot_sendStartMatch);

    connect(m_client, &Client::sig_gameStateUpdate,
            matchWindow, &Match::slot_updateEnemyFields);

    connect(m_client, &Client::sig_matchStarted,
            lobby, &Lobby::slot_startMatch);

    // Lobby -> Client
    connect(lobby, &Lobby::sig_isReady,
            m_client , &Client::slot_changeReadyState);

    // Lobby → Match
    connect(lobby, &Lobby::sig_startMatch, this, [this]() {
        ui->stack->setCurrentWidget(matchWindow);
    });

    // Lobby → zurück zum Menu
    connect(lobby, &Lobby::sig_backToMenu,
            this, &Menu::slot_backToMenu);


    // Match Form
    connect(matchWindow, &Match::sig_backToMenu,
            this, &Menu::slot_backToMenu);

    connect(matchWindow, &Match::sig_action,
            m_client, &Client::slot_sendMatchAction);


    //Menu Buttons
    connect(ui->replayBtn, &QPushButton::clicked,
            this, &Menu::on_replayBtn_clicked);
    connect(ui->shopBtn, &QPushButton::clicked,
            this, &Menu::on_shopBtn_clicked);
    connect(ui->settingsBtn, &QPushButton::clicked,
            this, &Menu::on_settingsBtn_clicked);
    connect(ui->playBtn, &QPushButton::clicked,
            this, &Menu::on_playBtn_clicked);
    connect(ui->quitGameBtn, &QPushButton::clicked,
            this, &Menu::on_quitGameBtn_clicked);
}

void Menu::m_setPlayerConfig(quint32 ID, quint16 token, QString name, bool force)
{
    m_client->m_updateInternalConfig(ID, token, name, force);
    SettingsWindow->m_setPlayerName(name);
}

void Menu::closeEvent(QCloseEvent *event)
{
    on_quitGameBtn_clicked();
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
    delete browserWindow;
}

void Menu::on_playBtn_clicked()
{
    //lobby->setPlayerName(1, SettingsWindow->m_getPlayerName());
    browserWindow->show();
}

void Menu::slot_BrowserConnect()
{
    browserWindow->hide();
    ui->stack->setCurrentWidget(lobby);
}

void Menu::slot_backToMenu()
{
    m_client->on_detach();
    ui->stack->setCurrentWidget(ui->menu);
}

void Menu::on_rulesBtn_clicked()
{
    rulesWindow->show();
}

void Menu::on_settingsBtn_clicked()
{
    SettingsWindow->show();
}

void Menu::on_replayBtn_clicked()
{
    replayWindow->show();
}

void Menu::on_shopBtn_clicked()
{
    shopWindow->show();
}


void Menu::on_quitGameBtn_clicked()
{
    m_client->on_detach();

    if (m_client->m_getTcpSocket()->state() == QAbstractSocket::UnconnectedState) {
        LOG_OUT << "Socket already closed. Quitting immediately." << Qt::endl;
        qApp->quit();
    } else {
        connect(m_client->m_getTcpSocket(),
        &QTcpSocket::disconnected, qApp, &QCoreApplication::quit);
        QTimer::singleShot(1000, qApp, &QCoreApplication::quit);
    }
}

