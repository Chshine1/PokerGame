#include "ChipPool.h"

ChipPool::ChipPool(const int initialChips) : chips_(initialChips) {
}

bool ChipPool::takeChips(const int amount) {
    if (amount <= chips_ && amount > 0) {
        chips_ -= amount;
        return true;
    }
    return false;
}

void ChipPool::addChips(const int amount) {
    if (amount > 0) {
        chips_ += amount;
    }
}

int ChipPool::getChips() const { return chips_; }
