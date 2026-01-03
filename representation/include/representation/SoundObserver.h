#ifndef PACMANGAME_SOUNDOBSERVER_H
#define PACMANGAME_SOUNDOBSERVER_H

#include "logic/patterns/Observer.h"
#include "logic/utils/Score.h"
#include "representation/SoundManager.h"

namespace representation {
/**
 * Observer that bridges game events (logic layer) to audio feedback (representation layer).
 *
 * Attached to Score subject in LevelState, receives notifications via onNotify()
 * whenever Score::setEvent() + scoreSubject.notify() is called.
 *
 * Event → Sound mapping:
 * - COIN_COLLECTED → continuous coin loop (starts/extends timeout)
 * - FRUIT_EATEN → fruit eat sound
 * - GHOST_EATEN → ghost eaten sound
 * - GHOST_FEAR_MODE → fear mode sound
 * - PACMAN_DEATH_ANIM → stops coin loop + plays death sound
 *
 * Inline implementation kept in header for simplicity (small switch-case logic).
 */
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
            // Stop looping coin sound before death sound to prevent audio overlap
            SoundManager::getInstance().stopCoinSound();
            SoundManager::getInstance().playSound(SoundEffect::DEATH);
            break;
        default:
            break;
        }
    }
};
} // namespace representation

#endif // PACMANGAME_SOUNDOBSERVER_H