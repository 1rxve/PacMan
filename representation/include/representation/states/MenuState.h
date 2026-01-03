#ifndef PACMANGAME_MENUSTATE_H
#define PACMANGAME_MENUSTATE_H

#include "State.h"
#include "representation/SoundManager.h"
#include <string>

namespace representation {
/**
 * Initial state displaying title, high scores, and start prompt.
 *
 * Features:
 * - Menu music loop (managed via SoundManager)
 * - Top 5 high scores from persistent storage
 * - Blinking "Press SPACE to start" instruction
 * - Dynamic high score refresh on return from gameplay
 *
 * State transitions:
 * - SPACE → LevelState (push)
 * - ESC → close window
 */
class MenuState : public State {
private:
    std::string mapFile;
    sf::Font font;
    sf::Text titleText;
    sf::Text instructionText;
    sf::Text highScoresTitle;
    sf::Text highScoresText[5];
    bool fontLoaded;
    bool needsRefresh;

    float blinkTimer;
    bool instructionVisible;

    /**
     * Reloads high scores from file and updates UI text elements.
     * Called on update() to reflect new scores after gameplay.
     */
    void refreshHighScores();

public:
    MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
              const std::string& mapFile);

    ~MenuState();

    void update(float deltaTime) override;

    void render() override;

    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_MENUSTATE_H