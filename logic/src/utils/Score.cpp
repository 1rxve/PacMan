#include "logic/utils/Score.h"
#include <algorithm>
#include <iostream>
#include <fstream>

namespace logic {
    Score::Score()
            : score(0),
              timeSinceLastCoin(0.0f),
              accumulatedDecay(0.0f),
              lastEvent(ScoreEvent::COIN_COLLECTED) {
    }

    void Score::setEvent(ScoreEvent event) {
        lastEvent = event;
    }

    void Score::onNotify() {
        switch (lastEvent) {
            case ScoreEvent::COIN_COLLECTED:
                handleCoinCollected();
                break;
            case ScoreEvent::GHOST_EATEN:
                // TODO: ghost bonus
                break;
            case ScoreEvent::FRUIT_EATEN:
                // TODO: fruit bonus
                break;
            case ScoreEvent::PACMAN_DIED:
                // TODO: death penalty
                break;
            case ScoreEvent::LEVEL_CLEARED:
                // TODO: level bonus
                break;
        }
    }

    void Score::handleCoinCollected() {
        float timeBonus = TIME_BONUS_MULTIPLIER / std::max(timeSinceLastCoin, 0.1f);
        int coinPoints = static_cast<int>(BASE_COIN_VALUE * (1.0f + timeBonus));

        score += coinPoints;
        timeSinceLastCoin = 0.0f;

        std::cout << "Score +=" << coinPoints << " (total: " << score << ")" << std::endl;
    }

    void Score::update(float deltaTime) {
        timeSinceLastCoin += deltaTime;

        // Accumulate fractional decay to prevent rounding issues
        accumulatedDecay += DECAY_RATE * deltaTime;

        // Apply integer decay when accumulated >= 1
        if (accumulatedDecay >= 1.0f) {
            int decayAmount = static_cast<int>(accumulatedDecay);
            score -= decayAmount;
            accumulatedDecay -= decayAmount;  // Keep fractional part

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

    int Score::getScore() const {
        return score;
    }

    std::string Score::getHighScoresFilePath() {
        return "resources/highscores/highscores.txt";
    }

    std::vector<Score::HighScoreEntry> Score::loadHighScores() {
        std::vector<HighScoreEntry> scores;

        std::ifstream file(getHighScoresFilePath());
        if (!file.is_open()) {
            return scores;  // File doesn't exist yet
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
                  [](const HighScoreEntry& a, const HighScoreEntry& b) {
                      return a.score > b.score;
                  });

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

        // Als er minder dan 5 scores zijn, is elke score een high score
        if (scores.size() < 5) {
            return true;
        }

        // Check of score hoger is dan de laagste in top 5
        return score > scores[4].score;  // scores[4] is de 5de (laagste)
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
}