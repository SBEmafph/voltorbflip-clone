classDiagram

%% ===== Shared / Network =====
class GameState {
    +QMap< quint8, PlayerState > playerList
}

class PlayerState {
    +bool m_fItemEquipped[3]
    +bool m_fRevealed[25]
    +quint8 m_bBoard[25]
    +quint8 m_bPlayerID
    +quint8 m_bLevel
    +quint8 m_bTotalScore
    +quint8 m_bCurrentScore
    +QString m_cPlayerName
}

class IObserver {
    <<interface>>
    +updateStates(state: GameState) void
}

class NetworkObserver {
    -socket: QTcpSocket*
    +NetworkObserver(QTcpSocket* clientSocket)
    +updateStates(const GameState &state) void
    +prepareDataForThisPlayer(const GameState &state) : void
}

IObserver <|.. NetworkObserver
GameState o-- PlayerState

%% ===== Server =====
class VoltOrbFlipServer {
    -QTcpServer* m_tcpServer 
    -GameState m_gameState 
    -QMap < int, NetworkObserver* > m_clients 
    +m_attach() void
    +m_detach() void
    +m_processInput(playerId, input)
    +m_verifyInput(enum input) bool
    +m_applyMove(enum input) void
    +m_notifyClients() void
    -m_findFirstFreeSlot() quint8
}

VoltOrbFlipServer --> GameState
VoltOrbFlipServer o--> IObserver
VoltOrbFlipServer <-- Client

%% ===== Client =====
class Client {
    -QTcpSocket m_tcpSocket
    -QDataStream m_in
    +sendInput(input)
    +receiveState()
    +m_attach() void
}

Client <--> NetworkObserver : TCP/IP

%% ===== UI (Qt Widgets) =====
class Menu{
    -Menu* Menu
    -Lobby* LobbyWindow
    -Rules* RulesWindow
    -Match* MatchWindow
    -Replay* ReplayWindow
    -on_playBtn_clicked() void
    on_rulesBtn_clicked() void
    on_replayBtn_clicked() void
}

class Lobby{
    -Ui::Lobby *ui
    -int secondsLeft
    -QTimer* matchTimer
    sig backToMenu() void
    sig startMatch() void
    -slt on_quitBtn_clicked()
    -slt onMatchTimeout()
    #showEvent(QShowEvent *event) void 
}

class Match{
    -UI::Match * ui
    sig backToMenu() void
    -slt on_quitBtn_clicked() void
}

class Rules{
    Ui::rules *ui
    sig backToMenu() void
    -slt on_okBtn_clicked() void
}

class Replay{
    Ui::replay *ui
    sig backToMenu() void
    -slt on_closeBtn_clicked() void
}

Client --> Menu : updates UI
Client --> Lobby : updates UI
Client --> Match : updates UI
Client --> Rules : updates UI
Client --> Replay : updates UI
