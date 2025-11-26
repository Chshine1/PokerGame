#ifndef TERMINAL_PLAYER_H
#define TERMINAL_PLAYER_H
#include <iostream>

#include "Player.h"

class TerminalPlayer final : public Player {
public:
    explicit TerminalPlayer(const std::string &name, int initialChips = 1000);

    Action makeDecision(int currentBet, int chipsCommitted,
                        const std::vector<Card> &communityCards) override;

    int getRaiseAmount(int currentBet, int _) override;
};
#endif
