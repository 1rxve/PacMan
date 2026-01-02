#ifndef PACMANGAME_SCORE_H
#define PACMANGAME_SCORE_H

#include "logic/patterns/Observer.h"
#include <string>
#include <vector>

namespace logic {
enum class ScoreEvent {
    COIN_COLLECTED,
    GHOST_EATEN,
    FRUIT_EATEN,
    PACMAN_DIED,
    LEVEL_CLEARED,
    GHOST_FEAR_MODE,
    PACMAN_DEATH_ANIM
};

class Score : public Observer {
private:
    int score;
    float timeSinceLastCoin;
    float accumulatedDecay;

    static constexpr int BASE_COIN_VALUE = 10;
    static constexpr float DECAY_RATE = 1.0f;
    static constexpr float TIME_BONUS_MULTIPLIER = 2.0f;

    ScoreEvent lastEvent;

    void handleCoinCollected();

    void handleGhostEaten();

    void handleFruitEaten();

    void handleLevelCleared();

public:
    Score();

    void onNotify() override; // ← Observer pattern
    void setEvent(ScoreEvent event);
    void update(float deltaTime);
    void reset();

    int getScore() const;

    ScoreEvent getLastEvent() const { return lastEvent; }

    // High score management
    struct HighScoreEntry {
        std::string name;
        int score;
    };

    static void saveHighScore(const std::string& name, int score);
    static std::vector<HighScoreEntry> loadHighScores();
    static std::string getHighScoresFilePath();
    static bool isHighScore(int score);
    static bool isTopScore(int score);
};
} // namespace logic

#endif