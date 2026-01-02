#ifndef PACMANGAME_SUBJECT_H
#define PACMANGAME_SUBJECT_H

#include "Observer.h"
#include <vector>

namespace logic {
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
