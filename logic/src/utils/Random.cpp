#include "logic/utils/Random.h"

namespace logic {
// Initialiseer static pointer
Random* Random::instance = nullptr;

// Private constructor met random seed
Random::Random() {
    std::random_device rd;
    generator.seed(rd());
}

// Static getter
Random& Random::getInstance() {
    if (instance == nullptr) {
        instance = new Random();
    }
    return *instance;
}

// Random int tussen min en max (inclusive)
int Random::getInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
}

// Random float tussen min en max
float Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator);
}
} // namespace logic