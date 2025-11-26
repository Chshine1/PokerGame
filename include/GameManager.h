#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <memory>
#include <vector>

#include "Player.h"

class GameManager {
public:
    explicit GameManager(int maxRounds = 100);

    void displayGameStatus(const std::vector<std::unique_ptr<Player> > &players) const;

    bool askToContinue();

    void recordRoundResult(const std::string &result);

    void saveGameHistory() const;

    void displayGameOver(const std::vector<std::unique_ptr<Player> > &players) const;

    bool shouldEndGame(const std::vector<std::unique_ptr<Player> > &players) const;

    int getRoundsPlayed() const;

    void endGame();

private:
    bool gameRunning_;
    int roundsPlayed_;
    int maxRounds_;
    std::vector<std::string> gameHistory_;
};
#endif
