#ifndef PACMANGAME_SCORE_H
#define PACMANGAME_SCORE_H

#include "logic/patterns/Observer.h"
#include <string>
#include <vector>

namespace logic {
/**
 * Game events that trigger score changes.
 */
enum class ScoreEvent {
    COIN_COLLECTED,    // +10 base, time bonus for quick collection
    GHOST_EATEN,       // +200 bonus
    FRUIT_EATEN,       // +100 bonus
    PACMAN_DIED,       // -500 penalty
    LEVEL_CLEARED,     // +500 bonus
    GHOST_FEAR_MODE,   // No score change (triggers sound only)
    PACMAN_DEATH_ANIM  // No score change (triggers sound only)
};

/**
 * Manages game score with time-based mechanics and persistent high scores.
 *
 * Scoring system:
 * - Coins: Base 10 points + time bonus (faster collection = higher multiplier)
 * - Score decays over time (1 point/second) to encourage fast play
 * - Events (ghost eaten, fruit, level clear) give fixed bonuses
 * - Death penalty: -500 points
 *
 * Implements Observer pattern to react to game events from World.
 * High scores persisted to file (top 5).
 */
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

    void handlePacManDied();


public:
    Score();

    void onNotify() override;

    void setEvent(ScoreEvent event);

    /**
     * Updates time-based score decay.
     *
     * Decreases score by DECAY_RATE per second to encourage fast gameplay.
     * Uses fractional accumulation to avoid rounding errors.
     *
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);

    void reset();

    int getScore() const;

    ScoreEvent getLastEvent() const { return lastEvent; }

    /**
    * High score entry (name + score pair).
    */
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