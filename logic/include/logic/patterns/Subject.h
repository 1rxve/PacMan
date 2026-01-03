#ifndef PACMANGAME_SUBJECT_H
#define PACMANGAME_SUBJECT_H

#include "Observer.h"
#include <vector>

namespace logic {
/**
 * Subject side of Observer pattern.
 *
 * Maintains list of observers and notifies them on state changes.
 * EntityModel inherits from Subject to notify views on updates.
 * Score also uses Subject to broadcast game events.
 *
 * Note: Stores raw pointers (observers don't own subjects, subjects don't own observers).
 */
class Subject {
private:
    std::vector<Observer*> observers;

public:
    virtual ~Subject() { observers.clear(); }

    void attach(Observer* observer);

    void detach(Observer* observer);

    void notify();
};
} // namespace logic

#endif // PACMANGAME_SUBJECT_H
