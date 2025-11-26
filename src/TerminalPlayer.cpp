#include "TerminalPlayer.h"

#include <iostream>

#include "Player.h"

TerminalPlayer::TerminalPlayer(const std::string &name, const int initialChips)
    : Player(name, initialChips) {
}

Player::Action TerminalPlayer::makeDecision(const int currentBet, const int chipsCommitted,
                                            const std::vector<Card> &communityCards) {
    if (isFolded()) return Action::FOLD;

    std::cout << "\n--- " << name_ << "'s Turn ---" << std::endl;
    std::cout << "Chips: " << chips_.getChips() << std::endl;
    std::cout << "Current bet: " << currentBet << ", You've committed: " << chipsCommitted << std::endl;

    std::cout << "Your hole cards: ";
    for (const auto &card: holeCards_) {
        std::cout << card.toString() << " ";
    }
    std::cout << std::endl;

    if (!communityCards.empty()) {
        std::cout << "Community cards: ";
        for (const auto &card: communityCards) {
            std::cout << card.toString() << " ";
        }
        std::cout << std::endl;
    }

    while (true) {
        std::cout << "Choose action: ";
        if (currentBet > chipsCommitted) {
            std::cout << "1-Fold, 2-Call (" << (currentBet - chipsCommitted) << "), 3-Raise: ";
        } else {
            std::cout << "1-Fold, 2-Check, 3-Raise: ";
        }

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                fold();
                return Action::FOLD;
            case 2: {
                if (currentBet > chipsCommitted) {
                    return Action::CALL;
                }
                return Action::CHECK;
            }
            case 3: return Action::RAISE;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
}

int TerminalPlayer::getRaiseAmount(const int currentBet, int _) {
    int raiseAmount;
    std::cout << "Enter raise amount (minimum " << currentBet + 1 << "): ";
    std::cin >> raiseAmount;
    return raiseAmount;
}
