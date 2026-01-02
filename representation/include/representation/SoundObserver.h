#ifndef PACMANGAME_SOUNDOBSERVER_H
#define PACMANGAME_SOUNDOBSERVER_H

#include "logic/patterns/Observer.h"
#include "logic/utils/Score.h"
#include "representation/SoundManager.h"

namespace representation {
    class SoundObserver : public logic::Observer {
    private:
        logic::Score* scoreObject;

    public:
        SoundObserver(logic::Score* score) : scoreObject(score) {}

        void onNotify() override {
            switch (scoreObject->getLastEvent()) {
                case logic::ScoreEvent::COIN_COLLECTED:
                    SoundManager::getInstance().playSound(SoundEffect::COIN_COLLECT);
                    break;
                case logic::ScoreEvent::FRUIT_EATEN:
                    SoundManager::getInstance().playSound(SoundEffect::FRUIT_EAT);
                    break;
                case logic::ScoreEvent::GHOST_EATEN:
                    SoundManager::getInstance().playSound(SoundEffect::GHOST_EATEN);
                    break;
                case logic::ScoreEvent::GHOST_FEAR_MODE:
                    SoundManager::getInstance().playSound(SoundEffect::GHOST_FEAR);
                    break;
                case logic::ScoreEvent::PACMAN_DEATH_ANIM:
                    SoundManager::getInstance().stopCoinSound();
                    SoundManager::getInstance().playSound(SoundEffect::DEATH);
                    break;
                default:
                    break;
            }
        }
    };
}

#endif //PACMANGAME_SOUNDOBSERVER_H
