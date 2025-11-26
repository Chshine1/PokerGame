#ifndef POT_DISPLAY_H
#define POT_DISPLAY_H
#include <iostream>
#include <memory>
#include <vector>

#include "Player.h"

class PotDisplay {
public:
    PotDisplay();

    void setMainPot(int amount);

    void addSidePot(int amount, const std::vector<int>& eligiblePlayers);

    void clearAllPots();

    void display(const std::vector<std::unique_ptr<Player>>& players) const;

    int getTotalPot() const;

    int getMainPot() const;

    size_t getSidePotCount() const;

    void displaySimple() const;

    void distributeToWinner(const std::string &playerName);

    void distributeToWinners(const std::vector<int>& winnerIndices,
                            const std::vector<std::string>& winnerNames);

private:
    int mainPot_;
    std::vector<std::pair<int, std::vector<int>>> sidePots_;
};

#endif