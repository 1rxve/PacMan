#include "representation/states/PausedState.h"
#include "representation/StateManager.h"
#include "representation/states/LevelState.h"
#include <iostream>

namespace representation {
    PausedState::PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                             const Camera* cam, StateManager* sm, State* levelState,
                             const std::string& mapFile)
            : State(win, fac, cam, sm), fontLoaded(false), levelStateBelow(levelState),
              mapFile(mapFile) {

        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Paused text
            pausedText.setFont(font);
            pausedText.setString("PAUSED");
            pausedText.setCharacterSize(80);
            pausedText.setFillColor(sf::Color::White);

            float pausedX = window->getSize().x / 2.0f - 120;
            pausedText.setPosition(pausedX, 200);

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("P = Resume | R = Restart | ESC = Menu");
            instructionText.setCharacterSize(30);
            instructionText.setFillColor(sf::Color::White);

            float instructionX = window->getSize().x / 2.0f - 130;
            instructionText.setPosition(instructionX, 350);
        }

    }

    void PausedState::update(float /*deltaTime*/) {
        // Nothing to update when paused
    }

    void PausedState::render() {
        // Render frozen level state underneath
        if (levelStateBelow) {
            levelStateBelow->render();
        }

        // Dark semi-transparent overlay (dimmed effect)
        sf::RectangleShape overlay(sf::Vector2f(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));  // Black with transparency
        window->draw(overlay);

        if (fontLoaded) {
            window->draw(pausedText);
            window->draw(instructionText);
        } else {
            // Fallback: white rectangles
            sf::RectangleShape pausedBox(sf::Vector2f(250, 100));
            pausedBox.setFillColor(sf::Color::White);
            pausedBox.setPosition(window->getSize().x / 2.0f - 125, 200);
            window->draw(pausedBox);
        }
    }

    void PausedState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed) {
            // P = resume game
            if (event.key.code == sf::Keyboard::P) {
                stateManager->popState();  // Pop PausedState, back to LevelState
            }

            // ESC = quit to main menu
            if (event.key.code == sf::Keyboard::Escape) {
                stateManager->popState();  // Pop PausedState
                stateManager->popState();  // Pop LevelState, back to MenuState
            }

            // R = restart level
            if (event.key.code == sf::Keyboard::R) {
                // CRITICAL: Copy all needed data BEFORE popping
                StateManager* sm = stateManager;
                sf::RenderWindow* win = window;
                logic::AbstractFactory* fac = factory;
                const Camera* cam = camera;
                std::string map = mapFile;

                sm->popState();  // Pop PausedState - 'this' is now DESTROYED
                sm->popState();  // Pop old LevelState
                // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

                // Push fresh LevelState with LOCAL copies
                sm->pushState(std::make_unique<LevelState>(
                        win, fac, cam, sm, map
                ));
            }
        }
    }
}