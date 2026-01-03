#ifndef PACMANGAME_OBSERVER_H
#define PACMANGAME_OBSERVER_H

namespace logic {
/**
 * Observer pattern interface.
 *
 * Observers receive notifications when Subject state changes.
 * Used for:
 * - View updates when Model changes (MVC pattern)
 * - Score updates on game events (coin collected, ghost eaten, etc.)
 */
class Observer {
public:
    virtual ~Observer() = default;

    virtual void onNotify() = 0;
};
} // namespace logic

#endif // PACMANGAME_OBSERVER_H
