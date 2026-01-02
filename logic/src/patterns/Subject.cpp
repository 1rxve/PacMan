#include "logic/patterns/Subject.h"
#include "algorithm"

namespace logic {
void Subject::attach(Observer* observer) { observers.push_back(observer); }

void Subject::detach(Observer* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::notify() {
    for (Observer* obs : observers) {
        obs->onNotify();
    }
}
} // namespace logic