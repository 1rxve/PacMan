#include "representation/states/MenuState.h"
#include "representation/states/LevelState.h"
#include "representation/StateManager.h"
#include <iostream>

namespace representation {
    MenuState::MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                         const Camera* cam, StateManager* sm,
                         const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile), fontLoaded(false) {

        // Try to load font (Windows path)
        if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
            std::cout << "MenuState: Font loaded successfully" << std::endl;
        } else {
            std::cerr << "MenuState: Could not load font, using default rendering" << std::endl;
        }

        if (fontLoaded) {
            // Title text
            titleText.setFont(font);
            titleText.setString("PACMAN");
            titleText.setCharacterSize(80);
            titleText.setFillColor(sf::Color::Yellow);

            // Center horizontally
            float titleX = window->getSize().x / 2.0f - 150;
            titleText.setPosition(titleX, 150);

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press SPACE to start");
            instructionText.setCharacterSize(30);
            instructionText.setFillColor(sf::Color::White);

            // Center horizontally
            float instructionX = window->getSize().x / 2.0f - 150;
            instructionText.setPosition(instructionX, 300);
        }

        std::cout << "MenuState: Initialized" << std::endl;
    }

    void MenuState::update(float /*deltaTime*/) {
        // Nothing to update in menu
    }

    void MenuState::render() {
        if (fontLoaded) {
            window->draw(titleText);
            window->draw(instructionText);
        } else {
            // Fallback: draw colored rectangles if no font
            sf::RectangleShape titleBox(sf::Vector2f(300, 100));
            titleBox.setFillColor(sf::Color::Yellow);
            titleBox.setPosition(window->getSize().x / 2.0f - 150, 150);
            window->draw(titleBox);

            sf::RectangleShape instructionBox(sf::Vector2f(300, 50));
            instructionBox.setFillColor(sf::Color::White);
            instructionBox.setPosition(window->getSize().x / 2.0f - 150, 300);
            window->draw(instructionBox);
        }
    }

    void MenuState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                std::cout << "MenuState: SPACE pressed, transitioning to LevelState" << std::endl;

                stateManager->pushState(std::make_unique<LevelState>(
                        window, factory, camera, stateManager, mapFile
                ));
            }

            // ← ADD: ESC closes game
            if (event.key.code == sf::Keyboard::Escape) {
                std::cout << "MenuState: ESC pressed, closing game" << std::endl;
                window->close();
            }
        }
    }
}