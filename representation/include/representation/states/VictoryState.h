#ifndef PACMANGAME_VICTORYSTATE_H
#define PACMANGAME_VICTORYSTATE_H

#include "State.h"
#include <string>

namespace representation {
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

        bool isHighScore;              // ← ADD
        float blinkTimer;              // ← ADD
        bool newHighScoreVisible;      // ← ADD

    public:
        VictoryState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                     const Camera* cam, StateManager* sm,
                     bool won, int finalScore, const std::string& mapFile);

        void update(float deltaTime) override;
        void render() override;
        void handleEvent(const sf::Event& event) override;
    };
}


#endif //PACMANGAME_VICTORYSTATE_H
