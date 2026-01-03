#include "representation/states/VictoryState.h"
#include "logic/utils/Score.h"
#include "representation/StateManager.h"
#include "representation/states/LevelState.h"
#include "representation/states/MenuState.h"

namespace representation {
VictoryState::VictoryState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                           bool won, int finalScore, const std::string& mapFile)
    : State(win, fac, cam, sm), won(won), finalScore(finalScore), mapFile(mapFile), fontLoaded(false),
      isHighScore(logic::Score::isTopScore(finalScore)), blinkTimer(0.0f), newHighScoreVisible(true) {

    if (font.loadFromFile("resources/fonts/joystix.otf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }

    if (fontLoaded) {
        scoreText.setFont(font);
        scoreText.setString("FINAL SCORE: " + std::to_string(finalScore));
        scoreText.setCharacterSize(32);
        scoreText.setFillColor(sf::Color::White);

        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
        scoreText.setPosition(window->getSize().x / 2.0f, 150);

        if (isHighScore) {
            newHighScoreText.setFont(font);
            newHighScoreText.setString("NEW HIGH SCORE");
            newHighScoreText.setCharacterSize(20);
            newHighScoreText.setFillColor(sf::Color::Yellow);

            sf::FloatRect nhsBounds = newHighScoreText.getLocalBounds();
            newHighScoreText.setOrigin(nhsBounds.width / 2.0f, nhsBounds.height / 2.0f);
            newHighScoreText.setPosition(window->getSize().x / 2.0f, 200);
        }

        // Always "GAME OVER" (red) regardless of won parameter
        titleText.setFont(font);
        titleText.setString("GAME OVER");
        titleText.setCharacterSize(140);
        titleText.setFillColor(sf::Color::Red);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
        titleText.setPosition(window->getSize().x / 2.0f, window->getSize().y / 2.0f - 100);

        restartText.setFont(font);
        restartText.setString("R - Restart");
        restartText.setCharacterSize(28);
        restartText.setFillColor(sf::Color::White);

        sf::FloatRect restartBounds = restartText.getLocalBounds();
        restartText.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
        restartText.setPosition(window->getSize().x / 2.0f, window->getSize().y - 200);

        quitText.setFont(font);
        quitText.setString("ESC - Main Menu");

        sf::FloatRect quitBounds = quitText.getLocalBounds();
        quitText.setOrigin(quitBounds.width / 2.0f, quitBounds.height / 2.0f);
        quitText.setPosition(window->getSize().x / 2.0f, window->getSize().y - 150);
    }
}

void VictoryState::update(float deltaTime) {
    if (isHighScore) {
        blinkTimer += deltaTime;

        // Blink "NEW HIGH SCORE" every 0.5 seconds
        if (blinkTimer >= 0.5f) {
            newHighScoreVisible = !newHighScoreVisible;
            blinkTimer = 0.0f;
        }
    }
}

void VictoryState::render() {
    // Semi-transparent dark overlay (alpha 200 = ~78% opacity)
    sf::RectangleShape overlay(
        sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window->draw(overlay);

    if (fontLoaded) {
        window->draw(scoreText);

        if (isHighScore && newHighScoreVisible) {
            window->draw(newHighScoreText);
        }

        window->draw(titleText);
        window->draw(restartText);
        window->draw(quitText);
    } else {
        // Fallback for missing fonts (won parameter only used here)
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
        // CRITICAL: Copy all needed data BEFORE popping
        StateManager* sm = stateManager;
        sf::RenderWindow* win = window;
        logic::AbstractFactory* fac = factory;
        const Camera* cam = camera;
        std::string map = mapFile;

        sm->popState(); // Pop VictoryState - 'this' is now DESTROYED
        sm->popState(); // Pop LevelState
        // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

        sm->pushState(std::make_unique<LevelState>(win, fac, cam, sm, map));
        return;
    }

    if (event.key.code == sf::Keyboard::Escape) {
        stateManager->popState();
        stateManager->popState();
    }
}
} // namespace representation