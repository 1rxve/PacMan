#ifndef PACMANGAME_MENUSTATE_H
#define PACMANGAME_MENUSTATE_H

#include "State.h"
#include <string>

namespace representation {
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

        float blinkTimer;      // ← ADD
        bool instructionVisible;

        void refreshHighScores();

    public:
        MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                  const Camera* cam, StateManager* sm,
                  const std::string& mapFile);

        ~MenuState();

        void update(float deltaTime) override;
        void render() override;
        void handleEvent(const sf::Event& event) override;
    };
}


#endif //PACMANGAME_MENUSTATE_H
