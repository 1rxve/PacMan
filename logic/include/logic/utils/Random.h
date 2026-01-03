#ifndef PACMANGAME_RANDOM_H
#define PACMANGAME_RANDOM_H

#include <random>

namespace logic {
/**
 * Singleton random number generator using Mersenne Twister.
 *
 * Provides centralized RNG for game logic (ghost AI, spawning, etc.).
 * Seeded once at construction for deterministic-within-session randomness.
 *
 * Thread-safety: Not thread-safe (single-threaded game loop).
 */
class Random {
private:
    static Random* instance;

    Random();

    std::mt19937 generator;

public:
    static Random& getInstance();

    Random(const Random&) = delete;

    Random& operator=(const Random&) = delete;

    Random(Random&&) = delete;

    Random& operator=(Random&&) = delete;

    /**
     * Generates random integer in range [min, max] (inclusive).
     */
    int getInt(int min, int max);

    /**
     * Generates random float in range [min, max) (max exclusive).
     */
    float getFloat(float min, float max);
};
} // namespace logic

#endif // PACMANGAME_RANDOM_H