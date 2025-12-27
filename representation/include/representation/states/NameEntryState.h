#ifndef PACMANGAME_NAMEENTRYSTATE_H
#define PACMANGAME_NAMEENTRYSTATE_H

#include "State.h"
#include <string>

namespace representation {
    class NameEntryState : public State {
    private:
        int finalScore;
        std::string mapFile;

        std::string playerName;  // 3 letters
        int currentLetterIndex;  // 0, 1, or 2

        sf::Font font;
        sf::Text titleText;
        sf::Text scoreText;
        sf::Text instructionText;
        sf::Text nameText;
        sf::Text cursorText;
        bool fontLoaded;

        float blinkTimer;
        bool cursorVisible;

        void updateNameDisplay();
        void incrementLetter();
        void decrementLetter();
        void saveAndContinue();

    public:
        NameEntryState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                       const Camera* cam, StateManager* sm,
                       int finalScore, const std::string& mapFile);

        void update(float deltaTime) override;
        void render() override;
        void handleEvent(const sf::Event& event) override;
    };
}


#endif //PACMANGAME_NAMEENTRYSTATE_H
