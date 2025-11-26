#include "GameSettings.h"

#include <iostream>

GameSettings::GameSettings() : initialChips_(1000), maxRounds_(50), difficulty_(1) {
}

void GameSettings::displaySettings() const {
    std::cout << "\n=== 游戏设置 ===" << std::endl;
    std::cout << "初始筹码: " << initialChips_ << std::endl;
    std::cout << "最大回合: " << maxRounds_ << std::endl;
    std::cout << "难度等级: " << difficulty_ << std::endl;
    std::cout << "================" << std::endl;
}

void GameSettings::configureSettings() {
    std::cout << "\n=== 配置游戏设置 ===" << std::endl;

    std::cout << "初始筹码 (" << initialChips_ << "): ";
    std::cin >> initialChips_;

    std::cout << "最大回合数 (" << maxRounds_ << "): ";
    std::cin >> maxRounds_;

    std::cout << "难度等级 (1-简单, 2-中等, 3-困难) (" << difficulty_ << "): ";
    std::cin >> difficulty_;

    std::cout << "设置已更新！" << std::endl;
    displaySettings();
}

int GameSettings::getInitialChips() const { return initialChips_; }

int GameSettings::getMaxRounds() const { return maxRounds_; }

int GameSettings::getDifficulty() const { return difficulty_; }
