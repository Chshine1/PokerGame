#include <memory>

#include "ComputerPlayer.h"
#include "Player.h"
#include "PokerTable.h"
#include "TerminalPlayer.h"

int main() {
    PokerTable table;

    table.addPlayer(std::unique_ptr<Player>(new TerminalPlayer("Player", 1000)));
    table.addPlayer(std::unique_ptr<Player>(new ComputerPlayer("Computer 1", 1000)));
    table.addPlayer(std::unique_ptr<Player>(new ComputerPlayer("Computer 2", 1000)));

    table.startGame();
    return 0;
}
