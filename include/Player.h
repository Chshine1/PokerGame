#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>

#include "Card.h"
#include "ChipPool.h"

class Player {
public:
    enum class Action { FOLD, CHECK, CALL, RAISE };

    explicit Player(std::string name, int initialChips = 1000);

    virtual ~Player() = default;

    virtual Action makeDecision(int currentBet, int chipsCommitted, const std::vector<Card> &communityCards) = 0;

    virtual int getRaiseAmount(int currentBet, int chipsCommitted) = 0;

    void receiveCard(Card card);

    void clearHand();

    bool takeChips(int amount);

    void addChips(int amount);

    void fold();

    bool isFolded() const;

    void resetFold();

    const std::string &getName() const;

    int getChipCount() const;

    const std::vector<Card> &getHoleCards() const;

    bool isAllIn() const;

protected:
    std::string name_;
    ChipPool chips_;
    std::vector<Card> holeCards_;
    bool folded_;
};
#endif
