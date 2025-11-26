#include "PotDisplay.h"

#include <iostream>
#include <memory>
#include <vector>

#include "Player.h"

PotDisplay::PotDisplay() : mainPot_(0) {
}

void PotDisplay::setMainPot(const int amount) {
    mainPot_ = amount;
}

void PotDisplay::addSidePot(int amount, const std::vector<int> &eligiblePlayers) {
    sidePots_.emplace_back(amount, eligiblePlayers);
}

void PotDisplay::clearAllPots() {
    mainPot_ = 0;
    sidePots_.clear();
}

void PotDisplay::display(const std::vector<std::unique_ptr<Player> > &players) const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "                   筹码池信息" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::cout << "主池: " << mainPot_ << " 筹码" << std::endl;

    if (!sidePots_.empty()) {
        for (size_t i = 0; i < sidePots_.size(); i++) {
            std::cout << "边池 " << (i + 1) << ": " << sidePots_[i].first << " 筹码 - 参与者: ";
            for (const size_t playerIndex: sidePots_[i].second) {
                if (playerIndex < players.size()) {
                    std::cout << players[playerIndex]->getName() << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    std::cout << "总池: " << getTotalPot() << " 筹码" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

int PotDisplay::getTotalPot() const {
    int total = mainPot_;
    for (const auto &sidePot: sidePots_) {
        total += sidePot.first;
    }
    return total;
}

int PotDisplay::getMainPot() const {
    return mainPot_;
}

size_t PotDisplay::getSidePotCount() const {
    return sidePots_.size();
}

void PotDisplay::displaySimple() const {
    std::cout << "💰 当前总池: " << getTotalPot() << " 筹码";
    if (!sidePots_.empty()) {
        std::cout << " (包含 " << sidePots_.size() << " 个边池)";
    }
    std::cout << std::endl;
}

void PotDisplay::distributeToWinner(const std::string &playerName) {
    const int totalWon = getTotalPot();
    std::cout << "\n🎉 " << playerName << " 赢得 ";

    if (!sidePots_.empty()) {
        std::cout << "总池 " << totalWon << " 筹码 (主池: " << mainPot_ << " + ";
        for (size_t i = 0; i < sidePots_.size(); i++) {
            std::cout << "边池" << (i + 1) << ": " << sidePots_[i].first;
            if (i < sidePots_.size() - 1) std::cout << " + ";
        }
        std::cout << ")";
    } else {
        std::cout << totalWon << " 筹码";
    }
    std::cout << std::endl;

    clearAllPots();
}

void PotDisplay::distributeToWinners(const std::vector<int> &winnerIndices,
                                     const std::vector<std::string> &winnerNames) {
    const int totalPot = getTotalPot();
    const int share = totalPot / winnerIndices.size();

    std::cout << "\n🤝 平局！获胜者: ";
    for (size_t i = 0; i < winnerNames.size(); i++) {
        std::cout << winnerNames[i];
        if (i < winnerNames.size() - 1) std::cout << ", ";
    }

    if (!sidePots_.empty()) {
        std::cout << "\n每人获得 " << share << " 筹码 (从总池 " << totalPot << " 筹码中分配)";
    } else {
        std::cout << "\n每人获得 " << share << " 筹码";
    }
    std::cout << std::endl;

    clearAllPots();
}
