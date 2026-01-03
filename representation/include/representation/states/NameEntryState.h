#ifndef PACMANGAME_NAMEENTRYSTATE_H
#define PACMANGAME_NAMEENTRYSTATE_H

#include "State.h"
#include <string>

namespace representation {
/**
 * Interactive state for entering player name when achieving high score.
 *
 * Features:
 * - 3-letter name input (A-Z only, uppercase)
 * - Backspace to delete current letter and move back
 * - Auto-advance to next position after typing
 * - Enter to save (fills trailing spaces with 'A')
 * - Blinking cursor for visual feedback
 *
 * State transitions:
 * - Enter (with valid name) → VictoryState (replace)
 *
 * CRITICAL: saveAndContinue() copies all needed data to locals BEFORE popState()
 * to prevent use-after-free (this object is destroyed during pop).
 */
class NameEntryState : public State {
private:
    int finalScore;
    std::string mapFile;

    std::string playerName;
    int currentLetterIndex;
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text instructionText;
    sf::Text nameText;
    sf::Text cursorText;
    sf::Text pressEnterText;
    sf::Text whenDoneText;
    bool fontLoaded;

    float blinkTimer;
    bool cursorVisible;
    bool isNewHighScore;

    /**
     * Updates cursor position based on currentLetterIndex.
     * Called after each letter input or backspace.
     */
    void updateNameDisplay();

    /**
     * Saves high score to file and transitions to VictoryState.
     *
     * CRITICAL: Copies all member data to local variables BEFORE popState()
     * because this object is destroyed when popped from state stack.
     */
    void saveAndContinue();

public:
    NameEntryState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                   int finalScore, const std::string& mapFile);

    void update(float deltaTime) override;

    void render() override;

    void handleEvent(const sf::Event& event) override;
};
} // namespace representation

#endif // PACMANGAME_NAMEENTRYSTATE_H