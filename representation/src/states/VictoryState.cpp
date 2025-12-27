#include "representation/states/VictoryState.h"
#include "representation/states/MenuState.h"
#include "representation/StateManager.h"
#include <iostream>

namespace representation {
    VictoryState::VictoryState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                               const Camera* cam, StateManager* sm,
                               bool won, int finalScore, const std::string& mapFile)
            : State(win, fac, cam, sm),
              won(won),
              finalScore(finalScore),
              mapFile(mapFile),
              fontLoaded(false) {

        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Title text (YOU WON / GAME OVER)
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

        std::cout << "VictoryState: " << (won ? "WIN" : "LOSE")
                  << " | Score: " << finalScore << std::endl;
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
        if (event.key.code == sf::Keyboard::R) {
            std::cout << "VictoryState: Restart pressed" << std::endl;

            stateManager->popState();  // Pop VictoryState
            stateManager->popState();  // Pop LevelState

            // Safety: recreate MenuState if stack is empty
            if (stateManager->isEmpty()) {
                std::cout << "Stack empty - recreating MenuState" << std::endl;
                stateManager->pushState(std::make_unique<MenuState>(
                        window, factory, camera, stateManager, mapFile
                ));
            }
        }
    }
}
