#include "representation/states/PausedState.h"
#include "representation/StateManager.h"
#include "representation/states/LevelState.h"
#include <iostream>

namespace representation {
PausedState::PausedState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                         State* levelState, const std::string& mapFile)
    : State(win, fac, cam, sm), fontLoaded(false), levelStateBelow(levelState), mapFile(mapFile) {

    if (font.loadFromFile("resources/fonts/joystix.otf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }

    if (fontLoaded) {
        pausedText.setFont(font);
        pausedText.setString("PAUSED");
        pausedText.setCharacterSize(120);
        pausedText.setFillColor(sf::Color::White);

        sf::FloatRect pausedBounds = pausedText.getLocalBounds();
        pausedText.setOrigin(pausedBounds.width / 2.0f, pausedBounds.height / 2.0f);
        pausedText.setPosition(window->getSize().x / 2.0f, 250);

        resumeText.setFont(font);
        resumeText.setString("P - Resume");
        resumeText.setCharacterSize(28);
        resumeText.setFillColor(sf::Color::White);
        sf::FloatRect resumeBounds = resumeText.getLocalBounds();
        resumeText.setOrigin(resumeBounds.width / 2.0f, resumeBounds.height / 2.0f);
        resumeText.setPosition(window->getSize().x / 2.0f, 450);

        restartText.setFont(font);
        restartText.setString("R - Restart");
        restartText.setCharacterSize(28);
        restartText.setFillColor(sf::Color::White);
        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        restartText.setPosition(window->getSize().x / 2.0f, 500);

        quitText.setFont(font);
        quitText.setString("ESC - Quit to Main Menu");
        quitText.setCharacterSize(28);
        quitText.setFillColor(sf::Color::White);
        sf::FloatRect quitBounds = quitText.getLocalBounds();
        quitText.setOrigin(quitBounds.width / 2.0f, quitBounds.height / 2.0f);
        quitText.setPosition(window->getSize().x / 2.0f, 550);
    }
}

void PausedState::update(float /*deltaTime*/) {}

void PausedState::render() {
    // Render frozen LevelState underneath for visual context
    if (levelStateBelow) {
        levelStateBelow->render();
    }

    // Semi-transparent dark overlay (alpha 200 = ~78% opacity)
    sf::RectangleShape overlay(
        sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window->draw(overlay);

    if (fontLoaded) {
        window->draw(pausedText);
        window->draw(resumeText);
        window->draw(restartText);
        window->draw(quitText);
    } else {
        // Fallback for missing fonts (development/debugging aid)
        sf::RectangleShape pausedBox(sf::Vector2f(250, 100));
        pausedBox.setFillColor(sf::Color::White);
        pausedBox.setPosition(window->getSize().x / 2.0f - 125, 200);
        window->draw(pausedBox);
    }
}

void PausedState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P) {
            stateManager->popState();
        }

        if (event.key.code == sf::Keyboard::Escape) {
            stateManager->popState();
            stateManager->popState();
        }

        if (event.key.code == sf::Keyboard::R) {
            // CRITICAL: Copy all needed data BEFORE popping
            StateManager* sm = stateManager;
            sf::RenderWindow* win = window;
            logic::AbstractFactory* fac = factory;
            const Camera* cam = camera;
            std::string map = mapFile;

            sm->popState(); // Pop PausedState - 'this' is now DESTROYED
            sm->popState(); // Pop old LevelState
            // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

            sm->pushState(std::make_unique<LevelState>(win, fac, cam, sm, map));
        }
    }
}
} // namespace representation