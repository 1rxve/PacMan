#ifndef PACMANGAME_LEVELSTATE_H
#define PACMANGAME_LEVELSTATE_H

#include "State.h"
#include "logic/world/World.h"
#include "representation/SoundObserver.h"
#include <memory>
#include <string>

namespace representation {
/**
 * Active gameplay state managing World logic, UI overlay, and input handling.
 *
 * Responsibilities:
 * - Owns World instance (game logic controller)
 * - Renders HUD (score, lives, level number)
 * - Countdown timer between deaths/level transitions
 * - Cheat code detection (text input buffering)
 * - Sound event observation via SoundObserver
 *
 * State transitions:
 * - P key → PausedState (push)
 * - All coins collected → next level (internal reset)
 * - Lives depleted → VictoryState or NameEntryState (push)
 */
class LevelState : public State {
private:
    std::unique_ptr<logic::World> world;
    std::string mapFile;

    sf::Font font;
    sf::Text scoreText;
    sf::Text readyText;
    bool fontLoaded;

    std::shared_ptr<sf::Texture> livesTexture;
    sf::Sprite livesSprite;

    bool isCountingDown;
    float countdownTimer;

    std::string cheatBuffer;

    std::unique_ptr<SoundObserver> soundObserver;

public:
    LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
               const std::string& mapFile);

    ~LevelState();

    void update(float deltaTime) override;

    void render() override;

    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_LEVELSTATE_H