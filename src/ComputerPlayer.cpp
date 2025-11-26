#include "ComputerPlayer.h"

#include <random>
#include <iostream>

#include "Player.h"

ComputerPlayer::ComputerPlayer(const std::string &name, const int initialChips)
    : Player(name, initialChips) {
}

Player::Action ComputerPlayer::makeDecision(const int currentBet, const int chipsCommitted,
                                            const std::vector<Card> &_) {
    if (isFolded()) return Action::FOLD;

    std::cout << name_ << " is thinking..." << std::endl;

    std::random_device rd_;
    std::mt19937 gen(rd_());
    std::uniform_int_distribution dist(1, 100);
    const auto rand = dist(gen);

    if (currentBet > chipsCommitted) {
        if (rand % 100 < 70) {
            std::cout << name_ << " calls." << std::endl;
            return Action::CALL;
        }
        fold();
        std::cout << name_ << " folds." << std::endl;
        return Action::FOLD;
    }
    if (rand % 100 < 50) {
        return Action::RAISE;
    }
    std::cout << name_ << " checks." << std::endl;
    return Action::CHECK;
}

int ComputerPlayer::getRaiseAmount(const int currentBet, int _) {
    const auto raiseAmount = currentBet + currentBet / 2 + 10;
    std::cout << name_ << " raises to " << raiseAmount << "." << std::endl;
    return raiseAmount;
}
