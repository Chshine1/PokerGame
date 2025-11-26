
#include "PokerTable.h"

#include <memory>
#include <random>

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

            players_.erase(std::remove_if(players_.begin(), players_.end(),
                [](const std::unique_ptr<Player>& p) {
                    return p->getChipCount() <= 0;
                }), players_.end());

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
        for (int s = 0; s < 4; s++) {
            for (int r = 2; r <= 14; r++) {
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

    int PokerTable::evaluateHandStrength(const std::vector<Card>& holeCards, const std::vector<Card>& communityCards) {
        std::vector<Card> allCards = holeCards;
        allCards.insert(allCards.end(), communityCards.begin(), communityCards.end());

        std::ranges::sort(allCards,
                          [](const Card& a, const Card& b) {
                              return static_cast<int>(a.getRank()) > static_cast<int>(b.getRank());
                          });

        int score = 0;

        std::map<Card::Suit, int> suitCount;
        std::map<Card::Suit, std::vector<Card>> suitCards;
        for (const auto& card : allCards) {
            suitCount[card.getSuit()]++;
            suitCards[card.getSuit()].push_back(card);
        }

        for (const auto& suitEntry : suitCards) {
            Card::Suit suit = suitEntry.first;
            const std::vector<Card>& cards = suitEntry.second;
            if (cards.size() >= 5) {
                int straightCount = 1;
                for (size_t i = 1; i < cards.size(); i++) {
                    if (static_cast<int>(cards[i-1].getRank()) == static_cast<int>(cards[i].getRank()) + 1) {
                        straightCount++;
                        if (straightCount >= 5) {
                            score = 8000000 + static_cast<int>(cards[i-4].getRank()) * 10000;
                            return score;
                        }
                    } else if (static_cast<int>(cards[i-1].getRank()) != static_cast<int>(cards[i].getRank())) {
                        straightCount = 1;
                    }
                }
                score = 5000000 + static_cast<int>(cards[0].getRank()) * 10000;
            }
        }

        std::map<Card::Rank, int> rankCount;
        for (const auto& card : allCards) {
            rankCount[card.getRank()]++;
        }

        for (const auto& rankEntry : rankCount) {
            Card::Rank rank = rankEntry.first;
            int count = rankEntry.second;
            if (count == 4) {
                int kicker = 0;
                for (const auto& card : allCards) {
                    if (card.getRank() != rank) {
                        kicker = std::max(kicker, static_cast<int>(card.getRank()));
                    }
                }
                score = 7000000 + static_cast<int>(rank) * 10000 + kicker;
                return score;
            }
        }

        bool hasThree = false;
        Card::Rank threeRank = Card::Rank::TWO;
        bool hasTwo = false;
        Card::Rank twoRank = Card::Rank::TWO;

        for (const auto& rankEntry : rankCount) {
            Card::Rank rank = rankEntry.first;
            int count = rankEntry.second;
            if (count == 3) {
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
            score = 6000000 + static_cast<int>(threeRank) * 10000 + static_cast<int>(twoRank);
            return score;
        }

        std::vector<int> uniqueRanks;
        for (const auto& card : allCards) {
            uniqueRanks.push_back(static_cast<int>(card.getRank()));
        }
        std::sort(uniqueRanks.begin(), uniqueRanks.end());
        uniqueRanks.erase(std::unique(uniqueRanks.begin(), uniqueRanks.end()), uniqueRanks.end());

        int straightLength = 1;
        for (size_t i = 1; i < uniqueRanks.size(); i++) {
            if (uniqueRanks[i] == uniqueRanks[i-1] + 1) {
                straightLength++;
                if (straightLength >= 5) {
                    score = 4000000 + uniqueRanks[i] * 10000;
                    return score;
                }
            } else {
                straightLength = 1;
            }
        }

        if (hasThree) {
            std::vector<int> kickers;
            for (const auto& card : allCards) {
                if (card.getRank() != threeRank) {
                    kickers.push_back(static_cast<int>(card.getRank()));
                }
            }
            std::sort(kickers.begin(), kickers.end(), std::greater<int>());
            int kickerScore = 0;
            if (kickers.size() >= 2) {
                kickerScore = kickers[0] * 100 + kickers[1];
            } else if (kickers.size() == 1) {
                kickerScore = kickers[0] * 100;
            }
            score = 3000000 + static_cast<int>(threeRank) * 10000 + kickerScore;
            return score;
        }

        std::vector<Card::Rank> pairs;
        for (const auto& rankEntry : rankCount) {
            Card::Rank rank = rankEntry.first;
            int count = rankEntry.second;
            if (count == 2) {
                pairs.push_back(rank);
            }
        }

        if (pairs.size() >= 2) {
            std::sort(pairs.begin(), pairs.end(),
                [](Card::Rank a, Card::Rank b) {
                    return static_cast<int>(a) > static_cast<int>(b);
                });
            int kicker = 0;
            for (const auto& card : allCards) {
                if (card.getRank() != pairs[0] && card.getRank() != pairs[1]) {
                    kicker = std::max(kicker, static_cast<int>(card.getRank()));
                }
            }
            score = 2000000 + static_cast<int>(pairs[0]) * 10000 + static_cast<int>(pairs[1]) * 100 + kicker;
            return score;
        }

        if (pairs.size() == 1) {
            std::vector<int> kickers;
            for (const auto& card : allCards) {
                if (card.getRank() != pairs[0]) {
                    kickers.push_back(static_cast<int>(card.getRank()));
                }
            }
            std::sort(kickers.begin(), kickers.end(), std::greater<int>());
            int kickerScore = 0;
            if (kickers.size() >= 3) {
                kickerScore = kickers[0] * 10000 + kickers[1] * 100 + kickers[2];
            }
            score = 1000000 + static_cast<int>(pairs[0]) * 10000 + kickerScore;
            return score;
        }

        std::vector<int> highCards;
        for (const auto& card : allCards) {
            highCards.push_back(static_cast<int>(card.getRank()));
        }
        std::sort(highCards.begin(), highCards.end(), std::greater<int>());
        score = highCards[0] * 10000 + highCards[1] * 100 + highCards[2];
        return score;
    }

    void PokerTable::determineWinner() {
        std::cout << "\n=== Showdown ===" << std::endl;

        std::vector<int> activePlayers;
        for (int i = 0; i < players_.size(); i++) {
            if (!players_[i]->isFolded()) {
                activePlayers.push_back(i);
            }
        }

        if (activePlayers.empty()) {
            std::cout << "No active players!" << std::endl;
            return;
        }

        if (activePlayers.size() == 1) {
            int winnerIndex = activePlayers[0];
            potDisplay_.distributeToWinner(winnerIndex, players_[winnerIndex]->getName());
            players_[winnerIndex]->addChips(pot_);
            gameManager_.recordRoundResult(players_[winnerIndex]->getName() + " 获胜");
            return;
        }

        for (int i : activePlayers) {
            std::cout << players_[i]->getName() << "'s hand: ";
            for (const auto& card : players_[i]->getHoleCards()) {
                std::cout << card.toString() << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "Community cards: ";
        for (const auto& card : communityCards_) {
            std::cout << card.toString() << " ";
        }
        std::cout << std::endl;

        std::vector<std::pair<int, int>> playerScores;

        for (int i : activePlayers) {
            int score = evaluateHandStrength(players_[i]->getHoleCards(), communityCards_);
            playerScores.emplace_back(i, score);
            std::cout << players_[i]->getName() << "'s hand score: " << score << std::endl;
        }

        auto winner = std::max_element(playerScores.begin(), playerScores.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.second < b.second;
            });

        int maxScore = winner->second;

        std::vector<int> winners;
        for (const auto& playerScore : playerScores) {
            if (playerScore.second == maxScore) {
                winners.push_back(playerScore.first);
            }
        }

        if (winners.size() == 1) {
            int winnerIndex = winners[0];
            potDisplay_.distributeToWinner(winnerIndex, players_[winnerIndex]->getName());
            players_[winnerIndex]->addChips(pot_);
            gameManager_.recordRoundResult(players_[winnerIndex]->getName() + " 获胜");
        } else {
            std::vector<std::string> winnerNames;
            for (int winnerIndex : winners) {
                winnerNames.push_back(players_[winnerIndex]->getName());
            }
            potDisplay_.distributeToWinners(winners, winnerNames);

            int share = pot_ / winners.size();
            for (int winnerIndex : winners) {
                players_[winnerIndex]->addChips(share);
            }
            gameManager_.recordRoundResult("平局: " + std::to_string(winners.size()) + " 人");
        }
    }

    void PokerTable::awardPot(const std::vector<bool> &folded) const {
        for (int i = 0; i < players_.size(); i++) {
            if (!folded[i]) {
                std::cout << players_[i]->getName() << " wins " << pot_ << " chips!" << std::endl;
                players_[i]->addChips(pot_);
                break;
            }
        }
    }

    bool PokerTable::bettingRound() {
        std::vector<int> chipsCommitted(players_.size(), 0);
        int currentBet = 0;
        std::vector<bool> acted(players_.size(), false);
        int startPlayer = 0;
        int currentPlayer = startPlayer;

        while (true) {
            bool allActedOrFolded = true;
            bool allBetsEqual = true;
            int referenceBet = -1;

            for (int i = 0; i < players_.size(); i++) {
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
                    case Player::Action::FOLD:
                        acted[currentPlayer] = true;
                        break;

                    case Player::Action::CHECK:
                        acted[currentPlayer] = true;
                        break;

                    case Player::Action::CALL: {
                        int callAmount = currentBet - chipsCommitted[currentPlayer];
                        if (player->takeChips(callAmount)) {
                            chipsCommitted[currentPlayer] += callAmount;
                            pot_ += callAmount;
                        } else {
                            int allInAmount = player->getChipCount();
                            chipsCommitted[currentPlayer] += allInAmount;
                            pot_ += allInAmount;
                            player->takeChips(allInAmount);
                        }
                        acted[currentPlayer] = true;
                        break;
                    }

                    case Player::Action::RAISE: {
                        int raiseAmount = player->getRaiseAmount(currentBet, chipsCommitted[currentPlayer]);
                        if (raiseAmount > currentBet) {
                            int totalNeeded = raiseAmount - chipsCommitted[currentPlayer];
                            if (player->takeChips(totalNeeded)) {
                                chipsCommitted[currentPlayer] = raiseAmount;
                                pot_ += totalNeeded;
                                currentBet = raiseAmount;
                                for (int i = 0; i < players_.size(); i++) {
                                    if (i != currentPlayer && !players_[i]->isFolded() && !players_[i]->isAllIn()) {
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
                for (int i = 0; i < players_.size(); i++) {
                    if (!players_[i]->isFolded()) activeCount++;
                }
                if (activeCount <= 1) {
                    return false;
                }
            }

            currentPlayer = (currentPlayer + 1) % players_.size();

            if (currentPlayer == startPlayer) {
                bool needAnotherRound = false;
                for (int i = 0; i < players_.size(); i++) {
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

        for (auto& player : players_) {
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
};