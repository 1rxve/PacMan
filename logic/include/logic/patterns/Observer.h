#ifndef PACMANGAME_OBSERVER_H
#define PACMANGAME_OBSERVER_H

namespace logic {
    class Observer {
    public:
        virtual ~Observer() = default;
        virtual void onNotify() = 0;
    };
}

#endif //PACMANGAME_OBSERVER_H
