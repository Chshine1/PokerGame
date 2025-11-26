#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

class GameSettings {
public:
    GameSettings();

    void displaySettings() const;

    void configureSettings();

    int getInitialChips() const;

    int getMaxRounds() const;

    int getDifficulty() const;

private:
    int initialChips_;
    int maxRounds_;
    int difficulty_;
};
#endif
