#include "representation/states/VictoryState.h"
#include "representation/states/MenuState.h"
#include "representation/StateManager.h"
#include "logic/utils/Score.h"


namespace representation {
    VictoryState::VictoryState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                               const Camera* cam, StateManager* sm,
                               bool won, int finalScore, const std::string& mapFile)
            : State(win, fac, cam, sm),
              won(won),
              finalScore(finalScore),
              mapFile(mapFile),
              fontLoaded(false) {

        if (won && finalScore > 0) {
            logic::Score::saveHighScore("YOU", finalScore);
        }

        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Title text
            titleText.setFont(font);
            titleText.setString(won ? "YOU WON!" : "GAME OVER");
            titleText.setCharacterSize(64);
            titleText.setFillColor(won ? sf::Color::Green : sf::Color::Red);

            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, 200);

            // Score text
            scoreText.setFont(font);
            scoreText.setString("FINAL SCORE: " + std::to_string(finalScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color::Yellow);

            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            scoreText.setPosition(window->getSize().x / 2.0f, 350);

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press R to restart\nPress ESC to quit");
            instructionText.setCharacterSize(24);
            instructionText.setFillColor(sf::Color::White);

            sf::FloatRect instrBounds = instructionText.getLocalBounds();
            instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
            instructionText.setPosition(window->getSize().x / 2.0f, 500);
        }
    }

    void VictoryState::update(float /*deltaTime*/) {
        // Nothing to update
    }

    void VictoryState::render() {
        // Dark overlay
        sf::RectangleShape overlay(sf::Vector2f(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window->draw(overlay);

        if (fontLoaded) {
            window->draw(titleText);
            window->draw(scoreText);
            window->draw(instructionText);
        } else {
            // Fallback rectangles
            sf::RectangleShape titleBox(sf::Vector2f(400, 100));
            titleBox.setFillColor(won ? sf::Color::Green : sf::Color::Red);
            titleBox.setPosition(window->getSize().x / 2.0f - 200, 200);
            window->draw(titleBox);
        }
    }

    void VictoryState::handleEvent(const sf::Event& event) {
        if (event.type != sf::Event::KeyPressed) {
            return;
        }

        if (event.key.code == sf::Keyboard::R) {
            // Copy pointer before destroying self
            StateManager* sm = stateManager;

            sm->popState();  // Pop VictoryState
            sm->popState();  // Pop LevelState

            return;
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            window->close();
        }
    }
}
