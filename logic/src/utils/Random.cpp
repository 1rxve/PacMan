#include "logic/utils/Random.h"

namespace logic {
Random* Random::instance = nullptr;

Random::Random() {
    std::random_device rd;
    generator.seed(rd());
}

Random& Random::getInstance() {
    if (instance == nullptr) {
        instance = new Random();
    }
    return *instance;
}

int Random::getInt(int min, int max) {
    // Range: [min, max] (both inclusive)
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
}

float Random::getFloat(float min, float max) {
    // Range: [min, max) (max exclusive)
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator);
}
} // namespace logic