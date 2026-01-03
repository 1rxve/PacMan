#include "logic/utils/Stopwatch.h"

namespace logic {
Stopwatch* Stopwatch::instance = nullptr;

Stopwatch::Stopwatch() : deltaTime(0.0f) { lastTime = std::chrono::steady_clock::now(); }

Stopwatch& Stopwatch::getInstance() {
    if (instance == nullptr) {
        instance = new Stopwatch();
    }
    return *instance;
}

void Stopwatch::restart() {
    lastTime = std::chrono::steady_clock::now();
    deltaTime = 0.0f;
}

void Stopwatch::update() {
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    deltaTime = elapsed.count();
    lastTime = currentTime;
}

float Stopwatch::getDeltaTime() const { return deltaTime; }
} // namespace logic