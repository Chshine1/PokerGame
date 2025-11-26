#ifndef POKER_TABLE_H
#define POKER_TABLE_H
#include <algorithm>
#include <iostream>
#include <map>

#include "GameManager.h"
#include "GameSettings.h"
#include "PotDisplay.h"

class PokerTable {
public:
    PokerTable();

    void addPlayer(std::unique_ptr<Player> player);

    void startGame();

private:
    std::vector<std::unique_ptr<Player>> players_;
    std::vector<Card> deck_;
    std::vector<Card> communityCards_;
    int pot_;
    PotDisplay potDisplay_;
    GameManager gameManager_;
    GameSettings gameSettings_;
    void showWelcomeScreen();

    void initializeDeck();

    void shuffleDeck();

    void dealHoleCards();

    int evaluateHandStrength(const std::vector<Card>& holeCards, const std::vector<Card>& communityCards);

    void determineWinner();

    void awardPot(const std::vector<bool> &folded) const;

    bool bettingRound();

    void playHand();

    void showCommunityCards() const;
};
#endif
