#include "logic/utils/Stopwatch.h"

namespace logic {
    // Initialiseer static pointer - MOET in .cpp file
    Stopwatch* Stopwatch::instance = nullptr;

    // Private constructor
    Stopwatch::Stopwatch()
            : deltaTime(0.0f) {
        lastTime = std::chrono::steady_clock::now();
    }

    // Static getter met lazy initialization
    Stopwatch& Stopwatch::getInstance() {
        if (instance == nullptr) {
            instance = new Stopwatch();
        }
        return *instance;
    }

    // Reset de timer
    void Stopwatch::restart() {
        lastTime = std::chrono::steady_clock::now();
        deltaTime = 0.0f;
    }

    // Update deltaTime - roep aan in game loop
    void Stopwatch::update() {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
    }

    // Get huidige deltaTime
    float Stopwatch::getDeltaTime() const {
        return deltaTime;
    }
}