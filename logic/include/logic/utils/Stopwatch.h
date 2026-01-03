#ifndef PACMANGAME_STOPWATCH_H
#define PACMANGAME_STOPWATCH_H

#include <chrono>

namespace logic {
/**
 * Singleton stopwatch for frame-independent game timing.
 *
 * Provides deltaTime (time elapsed between frames) for smooth movement
 * regardless of frame rate. Ensures consistent game speed across devices.
 *
 * Usage: Call update() each frame, use getDeltaTime() for movement calculations.
 */
class Stopwatch {
private:
    static Stopwatch* instance;

    Stopwatch();

    std::chrono::steady_clock::time_point lastTime;
    float deltaTime;

public:
    static Stopwatch& getInstance();

    Stopwatch(const Stopwatch&) = delete;

    Stopwatch& operator=(const Stopwatch&) = delete;

    Stopwatch(Stopwatch&&) = delete;

    Stopwatch& operator=(Stopwatch&&) = delete;

    void restart();

    void update();

    /**
     * Returns time elapsed since last update() call in seconds.
     *
     * Used to make movement frame-independent:
     * position += speed * deltaTime
     *
     * Ensures game runs at same speed on fast and slow devices.
     */
    float getDeltaTime() const;
};
} // namespace logic

#endif // PACMANGAME_STOPWATCH_H