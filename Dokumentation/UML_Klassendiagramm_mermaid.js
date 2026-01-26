classDiagram
class IObserver {
    <<interface>>
    +update(state: GameState) void
}

class Server {
    -clients: List<IObserver>
    -gameState: GameState
    +attach(obs: IObserver) void
    +detach(obs: IObserver) void
    +notifyClients() void
    +processInput(playerId: int, input: string) void
    +verifyInput(input: string) bool
    +applyMove(input: string) void
}

class Client {
    -playerId: int
    +sendInput(input: string) void
    +update(state: GameState) void
}

class ClientUI {
    +displayGame(state: GameState) void
    +handleUserInput() string
}

class GameState {
    +board: 
    +currentScore: int
    +totalScore: int
    +level: int
    +currentPlayer: int
    -saveState() : void
}

IObserver <|.. Client

Server o-- IObserver : clients
Server --> GameState

Client --> Server : sendInput()
Client --> ClientUI : update UI
