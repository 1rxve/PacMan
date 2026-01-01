#ifndef PACMANGAME_RANDOM_H
#define PACMANGAME_RANDOM_H

#include <random>

namespace logic {
    class Random {
    private:
        // Static instance pointer
        static Random* instance;

        // Private constructor
        Random();

        // Random number generator (Mersenne Twister)
        std::mt19937 generator;

    public:
        // Static getter
        static Random& getInstance();

        // Delete copy/move
        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;
        Random(Random&&) = delete;
        Random& operator=(Random&&) = delete;

        // Public methods
        int getInt(int min, int max);
        float getFloat(float min, float max);
    };
}

#endif // PACMANGAME_RANDOM_H