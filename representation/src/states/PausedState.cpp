#include "representation/states/PausedState.h"
#include "representation/StateManager.h"
#include <iostream>

namespace representation {
    PausedState::PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                             const Camera* cam, StateManager* sm)
            : State(win, fac, cam, sm), fontLoaded(false) {

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
            // In PausedState constructor
            instructionText.setString("Press P or ESC to resume");
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
        // Semi-transparent black overlay
        sf::RectangleShape overlay(sf::Vector2f(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));  // Black with transparency
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
            // P or ESC = resume
            if (event.key.code == sf::Keyboard::P ||
                event.key.code == sf::Keyboard::Escape) {
                stateManager->popState();
            }
        }
    }
}