#ifndef PACMANGAME_VICTORYSTATE_H
#define PACMANGAME_VICTORYSTATE_H

#include "State.h"
#include <string>

namespace representation {
/**
 * End-of-game state displaying final score and restart/quit options.
 *
 * Handles BOTH victory and game over scenarios:
 * - Victory: All coins collected (won=true)
 * - Game Over: Lives depleted (won=false)
 *
 * Features:
 * - Large red "GAME OVER" title regardless of win/lose
 * - Final score display at top
 * - Blinking "NEW HIGH SCORE" indicator if applicable
 * - Dark semi-transparent overlay over final game state
 *
 * State transitions:
 * - R → pop twice + push new LevelState (restart from level 1)
 * - ESC → pop twice (return to MenuState)
 *
 * CRITICAL: R key copies all needed data to locals BEFORE popState()
 * to prevent use-after-free (this object destroyed during pop).
 */
class VictoryState : public State {
private:
    bool won;
    int finalScore;
    std::string mapFile;

    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text newHighScoreText;
    sf::Text restartText;
    sf::Text quitText;
    bool fontLoaded;

    bool isHighScore;
    float blinkTimer;
    bool newHighScoreVisible;

public:
    /**
     * @param won - True if all coins collected, false if lives depleted
     *              (currently unused for rendering - always shows "GAME OVER")
     */
    VictoryState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm, bool won,
                 int finalScore, const std::string& mapFile);

    void update(float deltaTime) override;

    void render() override;

    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_VICTORYSTATE_H