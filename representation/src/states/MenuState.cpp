#include "representation/states/MenuState.h"
#include "representation/states/LevelState.h"
#include "representation/StateManager.h"
#include <iostream>

namespace representation {
    MenuState::MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                         const Camera* cam, StateManager* sm,
                         const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile), fontLoaded(false) {

        // Load custom font with fallback
        if (font.loadFromFile("resources/fonts/joystix.otf")) {  // ← JE FONT NAAM
            fontLoaded = true;
            std::cout << "MenuState: Custom font loaded" << std::endl;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
            std::cout << "MenuState: Fallback font loaded" << std::endl;
        } else {
            std::cerr << "MenuState: No font available" << std::endl;
        }

        if (fontLoaded) {
            // Title text
            titleText.setFont(font);
            titleText.setString("PAC-MAN");
            titleText.setCharacterSize(80);
            titleText.setFillColor(sf::Color::Yellow);
            titleText.setPosition(window->getSize().x / 2.0f - 200, 50);

            // High scores title
            highScoresTitle.setFont(font);
            highScoresTitle.setString("HIGH SCORES");
            highScoresTitle.setCharacterSize(40);
            highScoresTitle.setFillColor(sf::Color::Cyan);
            highScoresTitle.setPosition(window->getSize().x / 2.0f - 150, 200);

            // Top 5 high scores (placeholder data for now)
            std::string dummyScores[5] = {
                    "1. 5000",
                    "2. 4500",
                    "3. 4000",
                    "4. 3500",
                    "5. 3000"
            };

            for (int i = 0; i < 5; i++) {
                highScoresText[i].setFont(font);
                highScoresText[i].setString(dummyScores[i]);
                highScoresText[i].setCharacterSize(30);
                highScoresText[i].setFillColor(sf::Color::White);
                highScoresText[i].setPosition(window->getSize().x / 2.0f - 100, 280 + i * 50);
            }

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press SPACE to start");
            instructionText.setCharacterSize(30);
            instructionText.setFillColor(sf::Color::Green);
            instructionText.setPosition(window->getSize().x / 2.0f - 180, 600);
        }

        std::cout << "MenuState: Initialized" << std::endl;
    }

    void MenuState::update(float /*deltaTime*/) {
        // Nothing to update in menu
    }

    void MenuState::render() {
        if (fontLoaded) {
            window->draw(titleText);
            window->draw(highScoresTitle);

            // Draw all 5 high scores
            for (int i = 0; i < 5; i++) {
                window->draw(highScoresText[i]);
            }

            window->draw(instructionText);
        } else {
            // Fallback: colored rectangles
            sf::RectangleShape titleBox(sf::Vector2f(300, 100));
            titleBox.setFillColor(sf::Color::Yellow);
            titleBox.setPosition(window->getSize().x / 2.0f - 150, 50);
            window->draw(titleBox);
        }
    }

    void MenuState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                stateManager->pushState(std::make_unique<LevelState>(
                        window, factory, camera, stateManager, mapFile
                ));
            }

            if (event.key.code == sf::Keyboard::Escape) {
                window->close();
            }
        }
    }
}