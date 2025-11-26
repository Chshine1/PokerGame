#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H
#include <random>

#include "Player.h"

class ComputerPlayer final : public Player {
public:
    explicit ComputerPlayer(const std::string &name, int initialChips = 1000);

    Action makeDecision(int currentBet, int chipsCommitted, const std::vector<Card> &_) override;

    int getRaiseAmount(int currentBet, int _) override;
};
#endif
