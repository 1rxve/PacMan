#ifndef PACMANGAME_STOPWATCH_H
#define PACMANGAME_STOPWATCH_H

#include <chrono>

namespace logic {
class Stopwatch {
private:
    // Static instance pointer - de enige instantie die bestaat
    static Stopwatch* instance;

    // Private constructor - niemand kan new Stopwatch() doen
    Stopwatch();

    // Data members
    std::chrono::steady_clock::time_point lastTime;
    float deltaTime;

public:
    // Static getter - hoe je de singleton krijgt
    static Stopwatch& getInstance();

    // Delete copy/move constructors - voorkom kopiëren
    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;
    Stopwatch(Stopwatch&&) = delete;
    Stopwatch& operator=(Stopwatch&&) = delete;

    // Public methods
    void restart();
    void update();
    float getDeltaTime() const;
};
} // namespace logic

#endif // PACMANGAME_STOPWATCH_H