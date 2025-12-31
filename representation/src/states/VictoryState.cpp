#include "representation/states/VictoryState.h"
#include "representation/states/MenuState.h"
#include "representation/states/LevelState.h"
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
              fontLoaded(false),
              isHighScore(logic::Score::isTopScore(finalScore)),
              blinkTimer(0.0f),
              newHighScoreVisible(true) {


        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Score at top
            scoreText.setFont(font);
            scoreText.setString("FINAL SCORE: " + std::to_string(finalScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color::White);

            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            scoreText.setPosition(window->getSize().x / 2.0f, 150);

            // "NEW HIGH SCORE" text (small, blinking, below score)
            if (isHighScore) {
                newHighScoreText.setFont(font);
                newHighScoreText.setString("NEW HIGH SCORE");
                newHighScoreText.setCharacterSize(20);
                newHighScoreText.setFillColor(sf::Color::Yellow);

                sf::FloatRect nhsBounds = newHighScoreText.getLocalBounds();
                newHighScoreText.setOrigin(nhsBounds.width / 2.0f, nhsBounds.height / 2.0f);
                newHighScoreText.setPosition(window->getSize().x / 2.0f, 200);
            }

            // GAME OVER - huge, red, centered
            titleText.setFont(font);
            titleText.setString("GAME OVER");
            titleText.setCharacterSize(140);
            titleText.setFillColor(sf::Color::Red);

            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, window->getSize().y / 2.0f - 100);

            // R - Restart (bottom)
            restartText.setFont(font);
            restartText.setString("R - Restart");
            restartText.setCharacterSize(28);
            restartText.setFillColor(sf::Color::White);

            sf::FloatRect restartBounds = restartText.getLocalBounds();
            restartText.setOrigin(restartBounds.width / 2.0f, restartBounds.height / 2.0f);
            restartText.setPosition(window->getSize().x / 2.0f, window->getSize().y - 200);

            // ESC - Main Menu (below restart)
            quitText.setFont(font);
            quitText.setString("ESC - Main Menu");

            sf::FloatRect quitBounds = quitText.getLocalBounds();
            quitText.setOrigin(quitBounds.width / 2.0f, quitBounds.height / 2.0f);
            quitText.setPosition(window->getSize().x / 2.0f, window->getSize().y - 150);
        }
    }

    void VictoryState::update(float deltaTime) {
        // Blink "NEW HIGH SCORE" text
        if (isHighScore) {
            blinkTimer += deltaTime;

            if (blinkTimer >= 0.5f) {
                newHighScoreVisible = !newHighScoreVisible;
                blinkTimer = 0.0f;
            }
        }
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
            window->draw(scoreText);  // Top

            // Blinking "NEW HIGH SCORE"
            if (isHighScore && newHighScoreVisible) {
                window->draw(newHighScoreText);
            }

            window->draw(titleText);  // Center (GAME OVER)
            window->draw(restartText);  // Bottom
            window->draw(quitText);
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

        // R = Restart game (fresh LevelState)
        if (event.key.code == sf::Keyboard::R) {
            // Copy all needed data BEFORE popping
            StateManager* sm = stateManager;
            sf::RenderWindow* win = window;
            logic::AbstractFactory* fac = factory;
            const Camera* cam = camera;
            std::string map = mapFile;

            sm->popState();  // Pop VictoryState - 'this' is now DESTROYED
            sm->popState();  // Pop LevelState
            // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

            // Push fresh LevelState
            sm->pushState(std::make_unique<LevelState>(
                    win, fac, cam, sm, map
            ));
            return;
        }

        // ESC = Main menu
        if (event.key.code == sf::Keyboard::Escape) {
            stateManager->popState();  // Pop VictoryState
            stateManager->popState();  // Pop LevelState, back to MenuState
        }
    }
}
