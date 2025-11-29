#ifndef PACMANGAME_SUBJECT_H
#define PACMANGAME_SUBJECT_H

#include <vector>
#include "Observer.h"

namespace logic {
    class Subject {
    private:
        std::vector<Observer*> observers;

    public:
        void attach(Observer* observer);
        void detach(Observer* observer);
        void notify();
    };
}

#endif //PACMANGAME_SUBJECT_H
