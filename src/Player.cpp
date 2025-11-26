#include "Player.h"

Player::Player(std::string name, const int initialChips)
    : name_(std::move(name)), chips_(initialChips), folded_(false) {
}

void Player::receiveCard(const Card card) {
    holeCards_.push_back(card);
}

void Player::clearHand() {
    holeCards_.clear();
    folded_ = false;
}

bool Player::takeChips(const int amount) {
    return chips_.takeChips(amount);
}

void Player::addChips(const int amount) {
    chips_.addChips(amount);
}

void Player::fold() { folded_ = true; }

bool Player::isFolded() const { return folded_; }

void Player::resetFold() { folded_ = false; }

const std::string &Player::getName() const { return name_; }

int Player::getChipCount() const { return chips_.getChips(); }

const std::vector<Card> &Player::getHoleCards() const { return holeCards_; }

bool Player::isAllIn() const { return getChipCount() == 0; }
