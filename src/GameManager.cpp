#include "GameManager.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include "Player.h"

GameManager::GameManager(const int maxRounds) : gameRunning_(true), roundsPlayed_(0), maxRounds_(maxRounds) {
}

void GameManager::displayGameStatus(const std::vector<std::unique_ptr<Player> > &players) const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "                   游戏状态" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "已进行回合: " << roundsPlayed_ << " / " << maxRounds_ << std::endl;
    std::cout << "存活玩家: " << players.size() << " 人" << std::endl;

    std::vector<std::pair<std::string, int> > playerRankings;
    for (const auto &player: players) {
        playerRankings.emplace_back(player->getName(), player->getChipCount());
    }

    std::ranges::sort(playerRankings,
                      [](const auto &a, const auto &b) {
                          return a.second > b.second;
                      });

    std::cout << "\n玩家筹码排行:" << std::endl;
    for (size_t i = 0; i < playerRankings.size(); i++) {
        std::cout << i + 1 << ". " << playerRankings[i].first
                << ": " << playerRankings[i].second << " 筹码" << std::endl;
    }
    std::cout << std::string(50, '=') << std::endl;
}

bool GameManager::askToContinue() {
    if (roundsPlayed_ >= maxRounds_) {
        std::cout << "\n⚠️  已达到最大回合数 (" << maxRounds_ << ")，游戏结束！" << std::endl;
        return false;
    }

    if (!gameRunning_) {
        return false;
    }

    char choice;
    while (true) {
        std::cout << "\n是否继续下一回合？" << std::endl;
        std::cout << "1. 继续游戏" << std::endl;
        std::cout << "2. 显示游戏状态" << std::endl;
        std::cout << "3. 保存并退出" << std::endl;
        std::cout << "4. 立即退出" << std::endl;
        std::cout << "请选择 (1-4): ";

        std::cin >> choice;

        switch (choice) {
            case '1':
                roundsPlayed_++;
                return true;
            case '2':
                return askToContinue();
            case '3':
                saveGameHistory();
                gameRunning_ = false;
                return false;
            case '4':
                gameRunning_ = false;
                return false;
            default:
                std::cout << "无效选择，请重新输入！" << std::endl;
        }
    }
}

void GameManager::recordRoundResult(const std::string &result) {
    gameHistory_.push_back("回合 " + std::to_string(roundsPlayed_) + ": " + result);
}

void GameManager::saveGameHistory() const {
    std::cout << "\n💾 保存游戏历史..." << std::endl;
    std::cout << "=== 游戏历史 ===" << std::endl;
    for (const auto &record: gameHistory_) {
        std::cout << record << std::endl;
    }
    std::cout << "================" << std::endl;
}

void GameManager::displayGameOver(const std::vector<std::unique_ptr<Player> > &players) const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "                   🎯 游戏结束 🎯" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "总回合数: " << roundsPlayed_ << std::endl;

    if (!players.empty()) {
        const auto winner = std::ranges::max_element(players,
                                                     [](const auto &a, const auto &b) {
                                                         return a->getChipCount() < b->getChipCount();
                                                     });

        std::cout << "🏆 最终获胜者: " << (*winner)->getName()
                << " (" << (*winner)->getChipCount() << " 筹码)" << std::endl;

        std::cout << "\n最终排名:" << std::endl;
        std::vector<std::pair<std::string, int> > finalRankings;
        for (const auto &player: players) {
            finalRankings.emplace_back(player->getName(), player->getChipCount());
        }

        std::ranges::sort(finalRankings,
                          [](const auto &a, const auto &b) {
                              return a.second > b.second;
                          });

        for (size_t i = 0; i < finalRankings.size(); i++) {
            std::string medal;
            if (i == 0) medal = "🥇";
            else if (i == 1) medal = "🥈";
            else if (i == 2) medal = "🥉";
            else medal = std::to_string(i + 1) + ".";

            std::cout << medal << " " << finalRankings[i].first
                    << ": " << finalRankings[i].second << " 筹码" << std::endl;
        }
    }

    std::cout << std::string(50, '=') << std::endl;
}

bool GameManager::shouldEndGame(const std::vector<std::unique_ptr<Player> > &players) const {
    if (players.size() <= 1) {
        return true;
    }

    if (roundsPlayed_ >= maxRounds_) {
        return true;
    }

    return !gameRunning_;
}

int GameManager::getRoundsPlayed() const { return roundsPlayed_; }
void GameManager::endGame() { gameRunning_ = false; }
