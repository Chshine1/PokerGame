#include "PokerTable.h"

#include <memory>
#include <random>
#include <ranges>

#include "Player.h"

PokerTable::PokerTable() : pot_(0), gameManager_(50) {
    initializeDeck();
}

void PokerTable::addPlayer(std::unique_ptr<Player> player) {
    players_.push_back(std::move(player));
}

void PokerTable::startGame() {
    showWelcomeScreen();

    std::cout << "=== 德州扑克游戏开始 ===" << std::endl;

    while (players_.size() > 1 && !gameManager_.shouldEndGame(players_)) {
        playHand();

        std::erase_if(players_,
                      [](const std::unique_ptr<Player> &p) {
                          return p->getChipCount() <= 0;
                      });

        gameManager_.displayGameStatus(players_);

        if (players_.size() > 1 && !gameManager_.askToContinue()) {
            break;
        }
    }

    gameManager_.displayGameOver(players_);
    gameManager_.saveGameHistory();
}

void PokerTable::showWelcomeScreen() {
    std::cout << "==========================================" << std::endl;
    std::cout << "           🎰 德州扑克游戏 🎰           " << std::endl;
    std::cout << "==========================================" << std::endl;

    char choice;
    std::cout << "1. 开始游戏（默认设置）" << std::endl;
    std::cout << "2. 配置游戏设置" << std::endl;
    std::cout << "3. 退出游戏" << std::endl;
    std::cout << "请选择 (1-3): ";
    std::cin >> choice;

    if (choice == '2') {
        gameSettings_.configureSettings();
        gameManager_ = GameManager(gameSettings_.getMaxRounds());
    } else if (choice == '3') {
        exit(0);
    }

    gameSettings_.displaySettings();
}

void PokerTable::initializeDeck() {
    deck_.clear();
    for (auto s = 0; s < 4; s++) {
        for (auto r = 2; r <= 14; r++) {
            deck_.emplace_back(
                static_cast<Card::Suit>(s),
                static_cast<Card::Rank>(r)
            );
        }
    }
}

void PokerTable::shuffleDeck() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::ranges::shuffle(deck_, g);
}

void PokerTable::dealHoleCards() {
    for (const auto &player: players_) {
        player->clearHand();
        player->receiveCard(deck_.back());
        deck_.pop_back();
        player->receiveCard(deck_.back());
        deck_.pop_back();
    }
}

int PokerTable::evaluateHandStrength(const std::vector<Card> &holeCards, const std::vector<Card> &communityCards) {
    std::vector<Card> allCards = holeCards;
    allCards.insert(allCards.end(), communityCards.begin(), communityCards.end());

    std::ranges::sort(allCards,
                      [](const Card &a, const Card &b) {
                          return static_cast<int>(a.getRank()) > static_cast<int>(b.getRank());
                      });

    auto score = 0;

    std::map<Card::Suit, int> suitCount;
    std::map<Card::Suit, std::vector<Card> > suitCards;
    for (const auto &card: allCards) {
        suitCount[card.getSuit()]++;
        suitCards[card.getSuit()].push_back(card);
    }

    for (const auto &card: suitCards | std::views::values) {
        if (const std::vector<Card> &cards = card; cards.size() >= 5) {
            auto straightCount = 1;
            for (size_t i = 1; i < cards.size(); i++) {
                if (static_cast<int>(cards[i - 1].getRank()) == static_cast<int>(cards[i].getRank()) + 1) {
                    straightCount++;
                    if (straightCount >= 5) {
                        return 8000000 + static_cast<int>(cards[i - 4].getRank()) * 10000;
                    }
                } else if (static_cast<int>(cards[i - 1].getRank()) != static_cast<int>(cards[i].getRank())) {
                    straightCount = 1;
                }
            }
        }
    }

    std::map<Card::Rank, int> rankCount;
    for (const auto &card: allCards) {
        rankCount[card.getRank()]++;
    }

    for (const auto &[rk, index]: rankCount) {
        Card::Rank rank = rk;
        if (const auto count = index; count == 4) {
            auto kicker = 0;
            for (const auto &card: allCards) {
                if (card.getRank() != rank) {
                    kicker = std::max(kicker, static_cast<int>(card.getRank()));
                }
            }
            return 7000000 + static_cast<int>(rank) * 10000 + kicker;
        }
    }

    auto hasThree = false;
    auto threeRank = Card::Rank::TWO;
    auto hasTwo = false;
    auto twoRank = Card::Rank::TWO;

    for (const auto &[rk, index]: rankCount) {
        Card::Rank rank = rk;
        if (auto count = index; count == 3) {
            if (!hasThree || static_cast<int>(rank) > static_cast<int>(threeRank)) {
                hasThree = true;
                threeRank = rank;
            }
        } else if (count == 2) {
            if (!hasTwo || static_cast<int>(rank) > static_cast<int>(twoRank)) {
                hasTwo = true;
                twoRank = rank;
            }
        }
    }

    if (hasThree && hasTwo) {
        return 6000000 + static_cast<int>(threeRank) * 10000 + static_cast<int>(twoRank);
    }

    std::vector<int> uniqueRanks;
    for (const auto &card: allCards) {
        uniqueRanks.push_back(static_cast<int>(card.getRank()));
    }
    std::ranges::sort(uniqueRanks);
    uniqueRanks.erase(std::ranges::unique(uniqueRanks).begin(), uniqueRanks.end());

    auto straightLength = 1;
    for (size_t i = 1; i < uniqueRanks.size(); i++) {
        if (uniqueRanks[i] == uniqueRanks[i - 1] + 1) {
            straightLength++;
            if (straightLength >= 5) {
                return 4000000 + uniqueRanks[i] * 10000;
            }
        } else {
            straightLength = 1;
        }
    }

    if (hasThree) {
        std::vector<int> kickers;
        for (const auto &card: allCards) {
            if (card.getRank() != threeRank) {
                kickers.push_back(static_cast<int>(card.getRank()));
            }
        }
        std::ranges::sort(kickers, std::greater());
        auto kickerScore = 0;
        if (kickers.size() >= 2) {
            kickerScore = kickers[0] * 100 + kickers[1];
        } else if (kickers.size() == 1) {
            kickerScore = kickers[0] * 100;
        }
        return 3000000 + static_cast<int>(threeRank) * 10000 + kickerScore;
    }

    std::vector<Card::Rank> pairs;
    for (const auto &[rk, index]: rankCount) {
        Card::Rank rank = rk;
        if (auto count = index; count == 2) {
            pairs.push_back(rank);
        }
    }

    if (pairs.size() >= 2) {
        std::ranges::sort(pairs,
                          [](Card::Rank a, Card::Rank b) {
                              return static_cast<int>(a) > static_cast<int>(b);
                          });
        auto kicker = 0;
        for (const auto &card: allCards) {
            if (card.getRank() != pairs[0] && card.getRank() != pairs[1]) {
                kicker = std::max(kicker, static_cast<int>(card.getRank()));
            }
        }
        return 2000000 + static_cast<int>(pairs[0]) * 10000 + static_cast<int>(pairs[1]) * 100 + kicker;
    }

    if (pairs.size() == 1) {
        std::vector<int> kickers;
        for (const auto &card: allCards) {
            if (card.getRank() != pairs[0]) {
                kickers.push_back(static_cast<int>(card.getRank()));
            }
        }
        std::ranges::sort(kickers, std::greater());
        auto kickerScore = 0;
        if (kickers.size() >= 3) {
            kickerScore = kickers[0] * 10000 + kickers[1] * 100 + kickers[2];
        }
        return 1000000 + static_cast<int>(pairs[0]) * 10000 + kickerScore;
    }

    std::vector<int> highCards;
    for (const auto &card: allCards) {
        highCards.push_back(static_cast<int>(card.getRank()));
    }
    std::ranges::sort(highCards, std::greater());
    score = highCards[0] * 10000 + highCards[1] * 100 + highCards[2];
    return score;
}

void PokerTable::determineWinner() {
    std::cout << "\n=== Showdown ===" << std::endl;

    std::vector<int> activePlayers;
    for (size_t i = 0; i < players_.size(); i++) {
        if (!players_[i]->isFolded()) {
            activePlayers.push_back(static_cast<int>(i));
        }
    }

    if (activePlayers.empty()) {
        std::cout << "No active players!" << std::endl;
        return;
    }

    if (activePlayers.size() == 1) {
        const auto winnerIndex = activePlayers[0];
        potDisplay_.distributeToWinner(players_[winnerIndex]->getName());
        players_[winnerIndex]->addChips(pot_);
        gameManager_.recordRoundResult(players_[winnerIndex]->getName() + " 获胜");
        return;
    }

    for (const auto i: activePlayers) {
        std::cout << players_[i]->getName() << "'s hand: ";
        for (const auto &card: players_[i]->getHoleCards()) {
            std::cout << card.toString() << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Community cards: ";
    for (const auto &card: communityCards_) {
        std::cout << card.toString() << " ";
    }
    std::cout << std::endl;

    std::vector<std::pair<int, int> > playerScores;

    for (auto i: activePlayers) {
        auto score = evaluateHandStrength(players_[i]->getHoleCards(), communityCards_);
        playerScores.emplace_back(i, score);
        std::cout << players_[i]->getName() << "'s hand score: " << score << std::endl;
    }

    const auto winner = std::ranges::max_element(playerScores,
                                                 [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
                                                     return a.second < b.second;
                                                 });

    const int maxScore = winner->second;

    std::vector<int> winners;
    for (const auto &[score, index]: playerScores) {
        if (index == maxScore) {
            winners.push_back(score);
        }
    }

    if (winners.size() == 1) {
        const auto winnerIndex = winners[0];
        potDisplay_.distributeToWinner(players_[winnerIndex]->getName());
        players_[winnerIndex]->addChips(pot_);
        gameManager_.recordRoundResult(players_[winnerIndex]->getName() + " 获胜");
    } else {
        std::vector<std::string> winnerNames;
        for (const auto winnerIndex: winners) {
            winnerNames.push_back(players_[winnerIndex]->getName());
        }
        potDisplay_.distributeToWinners(winners, winnerNames);

        const auto share = pot_ / winners.size();
        for (const auto winnerIndex: winners) {
            players_[winnerIndex]->addChips(static_cast<int>(share));
        }
        gameManager_.recordRoundResult("平局: " + std::to_string(winners.size()) + " 人");
    }
}

void PokerTable::awardPot(const std::vector<bool> &folded) const {
    for (size_t i = 0; i < players_.size(); i++) {
        if (!folded[i]) {
            std::cout << players_[i]->getName() << " wins " << pot_ << " chips!" << std::endl;
            players_[i]->addChips(pot_);
            break;
        }
    }
}

bool PokerTable::bettingRound() {
    std::vector chipsCommitted(players_.size(), 0);
    auto currentBet = 0;
    std::vector acted(players_.size(), false);
    constexpr auto startPlayer = 0;
    auto currentPlayer = startPlayer;

    while (true) {
        auto allActedOrFolded = true;
        auto allBetsEqual = true;
        auto referenceBet = -1;

        for (size_t i = 0; i < players_.size(); i++) {
            if (!players_[i]->isFolded()) {
                if (!acted[i] && !players_[i]->isAllIn()) {
                    allActedOrFolded = false;
                }
                if (referenceBet == -1) {
                    referenceBet = chipsCommitted[i];
                } else if (chipsCommitted[i] != referenceBet) {
                    allBetsEqual = false;
                }
            }
        }

        if (allActedOrFolded && allBetsEqual) {
            break;
        }

        if (!players_[currentPlayer]->isFolded() && !acted[currentPlayer] && !players_[currentPlayer]->isAllIn()) {
            const auto &player = players_[currentPlayer];

            const Player::Action action = player->makeDecision(
                currentBet, chipsCommitted[currentPlayer], communityCards_);

            switch (action) {
                case Player::Action::CHECK:
                case Player::Action::FOLD:
                    acted[currentPlayer] = true;
                    break;

                case Player::Action::CALL: {
                    if (const auto callAmount = currentBet - chipsCommitted[currentPlayer]; player->takeChips(
                        callAmount)) {
                        chipsCommitted[currentPlayer] += callAmount;
                        pot_ += callAmount;
                    } else {
                        const auto allInAmount = player->getChipCount();
                        chipsCommitted[currentPlayer] += allInAmount;
                        pot_ += allInAmount;
                        player->takeChips(allInAmount);
                    }
                    acted[currentPlayer] = true;
                    break;
                }

                case Player::Action::RAISE: {
                    if (const auto raiseAmount = player->getRaiseAmount(currentBet, chipsCommitted[currentPlayer]);
                        raiseAmount > currentBet) {
                        if (const auto totalNeeded = raiseAmount - chipsCommitted[currentPlayer]; player->takeChips(
                            totalNeeded)) {
                            chipsCommitted[currentPlayer] = raiseAmount;
                            pot_ += totalNeeded;
                            currentBet = raiseAmount;
                            for (size_t i = 0; i < players_.size(); i++) {
                                if (static_cast<int>(i) != currentPlayer && !players_[i]->isFolded() && !players_[i]->
                                    isAllIn()) {
                                    acted[i] = false;
                                }
                            }
                        }
                    }
                    acted[currentPlayer] = true;
                    break;
                }
            }

            int activeCount = 0;
            for (const auto &plr: players_) {
                if (!plr->isFolded()) activeCount++;
            }
            if (activeCount <= 1) {
                return false;
            }
        }

        currentPlayer = (currentPlayer + 1) % players_.size();

        if (currentPlayer == startPlayer) {
            bool needAnotherRound = false;
            for (size_t i = 0; i < players_.size(); i++) {
                if (!players_[i]->isFolded() && !acted[i] && !players_[i]->isAllIn()) {
                    needAnotherRound = true;
                    break;
                }
            }
            if (!needAnotherRound) {
                break;
            }
        }
    }

    return true;
}

void PokerTable::playHand() {
    pot_ = 0;
    potDisplay_.clearAllPots();
    communityCards_.clear();
    initializeDeck();
    shuffleDeck();

    for (auto &player: players_) {
        player->resetFold();
    }

    dealHoleCards();

    potDisplay_.displaySimple();

    if (!bettingRound()) {
        potDisplay_.setMainPot(pot_);
        potDisplay_.displaySimple();

        std::vector<bool> folded(players_.size(), false);
        for (int i = 0; i < players_.size(); i++) {
            folded[i] = players_[i]->isFolded();
        }
        awardPot(folded);
        return;
    }

    deck_.pop_back();
    for (int i = 0; i < 3; i++) {
        communityCards_.push_back(deck_.back());
        deck_.pop_back();
    }
    std::cout << "\nFlop: ";
    showCommunityCards();

    potDisplay_.setMainPot(pot_);
    potDisplay_.displaySimple();

    if (!bettingRound()) {
        std::vector<bool> folded(players_.size(), false);
        for (int i = 0; i < players_.size(); i++) {
            folded[i] = players_[i]->isFolded();
        }
        awardPot(folded);
        return;
    }

    deck_.pop_back();
    communityCards_.push_back(deck_.back());
    deck_.pop_back();
    std::cout << "\nTurn: " << communityCards_.back().toString() << std::endl;

    if (!bettingRound()) {
        std::vector<bool> folded(players_.size(), false);
        for (int i = 0; i < players_.size(); i++) {
            folded[i] = players_[i]->isFolded();
        }
        awardPot(folded);
        return;
    }

    deck_.pop_back();
    communityCards_.push_back(deck_.back());
    deck_.pop_back();
    std::cout << "\nRiver: " << communityCards_.back().toString() << std::endl;

    if (!bettingRound()) {
        std::vector<bool> folded(players_.size(), false);
        for (int i = 0; i < players_.size(); i++) {
            folded[i] = players_[i]->isFolded();
        }
        awardPot(folded);
        return;
    }

    determineWinner();
}

void PokerTable::showCommunityCards() const {
    for (const auto &card: communityCards_) {
        std::cout << card.toString() << " ";
    }
    std::cout << std::endl;
}
