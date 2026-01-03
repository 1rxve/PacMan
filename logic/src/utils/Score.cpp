#include "logic/utils/Score.h"
#include <algorithm>
#include <fstream>

namespace logic {
Score::Score() : score(0), timeSinceLastCoin(0.0f), accumulatedDecay(0.0f), lastEvent(ScoreEvent::COIN_COLLECTED) {}

void Score::setEvent(ScoreEvent event) { lastEvent = event; }

void Score::onNotify() {
    switch (lastEvent) {
    case ScoreEvent::COIN_COLLECTED:
        handleCoinCollected();
        break;
    case ScoreEvent::GHOST_EATEN:
        handleGhostEaten();
        break;
    case ScoreEvent::FRUIT_EATEN:
        handleFruitEaten();
        break;
    case ScoreEvent::PACMAN_DIED:
        handlePacManDied();
        break;
    case ScoreEvent::LEVEL_CLEARED:
        handleLevelCleared();
        break;
    case ScoreEvent::GHOST_FEAR_MODE:
        break;
    case ScoreEvent::PACMAN_DEATH_ANIM:
        break;
    }
}

void Score::handleCoinCollected() {
    // Time bonus: faster collection = higher multiplier
    // Formula: base × (1 + 2 / timeSinceLastCoin)
    // Max protection: clamp minimum time to 0.1s to prevent division issues
    float timeBonus = TIME_BONUS_MULTIPLIER / std::max(timeSinceLastCoin, 0.1f);
    int coinPoints = static_cast<int>(BASE_COIN_VALUE * (1.0f + timeBonus));

    score += coinPoints;
    timeSinceLastCoin = 0.0f;
}

void Score::handleGhostEaten() {
    const int GHOST_BONUS = 200;
    score += GHOST_BONUS;
}

void Score::handleFruitEaten() {
    const int FRUIT_BONUS = 100;
    score += FRUIT_BONUS;
}

void Score::handleLevelCleared() {
    const int LEVEL_BONUS = 500;
    score += LEVEL_BONUS;
}

void Score::handlePacManDied() {
    const int DEATH_PENALTY = 500;
    score -= DEATH_PENALTY;

    if (score < 0) {
        score = 0;
    }
}

void Score::update(float deltaTime) {
    timeSinceLastCoin += deltaTime;

    // Accumulate fractional decay to prevent rounding issues
    accumulatedDecay += DECAY_RATE * deltaTime;

    // Apply integer decay when accumulated >= 1
    if (accumulatedDecay >= 1.0f) {
        int decayAmount = static_cast<int>(accumulatedDecay);
        score -= decayAmount;
        accumulatedDecay -= decayAmount; // Keep fractional part

        if (score < 0) {
            score = 0;
        }
    }
}

void Score::reset() {
    score = 0;
    timeSinceLastCoin = 0.0f;
    accumulatedDecay = 0.0f;
}

int Score::getScore() const { return score; }

std::string Score::getHighScoresFilePath() { return "resources/highscores/highscores.txt"; }

std::vector<Score::HighScoreEntry> Score::loadHighScores() {
    std::vector<HighScoreEntry> scores;

    std::ifstream file(getHighScoresFilePath());
    if (!file.is_open()) {
        return scores;
    }

    std::string name;
    int score;

    while (file >> name >> score) {
        if (scores.size() < 5) {
            scores.push_back({name, score});
        }
    }

    file.close();
    return scores;
}

void Score::saveHighScore(const std::string& name, int score) {
    std::vector<HighScoreEntry> scores = loadHighScores();

    scores.push_back({name, score});

    std::sort(scores.begin(), scores.end(),
              [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });

    if (scores.size() > 5) {
        scores.resize(5);
    }

    std::ofstream file(getHighScoresFilePath());
    if (!file.is_open()) {
        return;
    }

    for (const auto& entry : scores) {
        file << entry.name << " " << entry.score << "\n";
    }

    file.close();
}

bool Score::isHighScore(int score) {
    std::vector<HighScoreEntry> scores = loadHighScores();

    // Top 5 not full yet - any score qualifies
    if (scores.size() < 5) {
        return true;
    }

    // Check if better than 5th place (lowest in top 5)
    return score > scores[4].score;
}

bool Score::isTopScore(int score) {
    std::vector<HighScoreEntry> scores = loadHighScores();

    // Empty list = first score = top score
    if (scores.empty()) {
        return true;
    }

    // Check if higher than current #1
    return score > scores[0].score;
}
} // namespace logic