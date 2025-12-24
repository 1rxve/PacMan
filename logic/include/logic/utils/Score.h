#ifndef PACMANGAME_SCORE_H
#define PACMANGAME_SCORE_H

#include "logic/patterns/Observer.h"

namespace logic {
    enum class ScoreEvent {
        COIN_COLLECTED,
        GHOST_EATEN,
        FRUIT_EATEN,
        PACMAN_DIED,
        LEVEL_CLEARED
    };

    class Score : public Observer {
    private:
        int score;
        float timeSinceLastCoin;
        float accumulatedDecay;  // ← NIEUW: accumulate fractional decay

        static constexpr int BASE_COIN_VALUE = 10;
        static constexpr float DECAY_RATE = 1.0f;
        static constexpr float TIME_BONUS_MULTIPLIER = 2.0f;

        ScoreEvent lastEvent;

    public:
        Score();

        void onNotify() override;  // ← Observer pattern
        void setEvent(ScoreEvent event);
        void update(float deltaTime);
        void reset();

        int getScore() const;

    private:
        void handleCoinCollected();
    };
}

#endif