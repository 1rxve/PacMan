#include "representation/states/MenuState.h"
#include "representation/states/LevelState.h"
#include "representation/StateManager.h"
#include "logic/utils/Score.h"

namespace representation {
    MenuState::MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                         const Camera* cam, StateManager* sm,
                         const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile), fontLoaded(false) {

        // Load custom font with fallback
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Title text
            titleText.setFont(font);
            titleText.setString("PAC-MAN");
            titleText.setCharacterSize(80);
            titleText.setFillColor(sf::Color::Yellow);

            // Center using bounds
            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, 100);

            // High scores title
            highScoresTitle.setFont(font);
            highScoresTitle.setString("HIGH SCORES");
            highScoresTitle.setCharacterSize(40);
            highScoresTitle.setFillColor(sf::Color::Cyan);

            sf::FloatRect hsBounds = highScoresTitle.getLocalBounds();
            highScoresTitle.setOrigin(hsBounds.width / 2.0f, hsBounds.height / 2.0f);
            highScoresTitle.setPosition(window->getSize().x / 2.0f, 250);

            // Top 5 high scores (centered)
            auto highScores = logic::Score::loadHighScores();

            for (int i = 0; i < 5; i++) {
                highScoresText[i].setFont(font);

                if (i < static_cast<int>(highScores.size())) {
                    std::string text = std::to_string(i + 1) + ". " +
                                       highScores[i].name + " - " +
                                       std::to_string(highScores[i].score);
                    highScoresText[i].setString(text);
                } else {
                    std::string text = std::to_string(i + 1) + ". --- - 0";
                    highScoresText[i].setString(text);
                }

                highScoresText[i].setCharacterSize(30);
                highScoresText[i].setFillColor(sf::Color::White);

                // Center each score line
                sf::FloatRect scoreBounds = highScoresText[i].getLocalBounds();
                highScoresText[i].setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
                highScoresText[i].setPosition(window->getSize().x / 2.0f, 330 + i * 50);
            }

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press SPACE to start");
            instructionText.setCharacterSize(30);
            instructionText.setFillColor(sf::Color::Green);

            sf::FloatRect instrBounds = instructionText.getLocalBounds();
            instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
            instructionText.setPosition(window->getSize().x / 2.0f, 650);
        }

        needsRefresh = false;
    }

    MenuState::~MenuState() {
    }

    void MenuState::update(float /*deltaTime*/) {
        // Check if we need to refresh high scores (after returning from game)
        if (!needsRefresh) {
            needsRefresh = true;  // Set flag for next time
        } else {
            refreshHighScores();
            needsRefresh = false;
        }
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

    void MenuState::refreshHighScores() {
        if (!fontLoaded) return;

        auto highScores = logic::Score::loadHighScores();

        for (int i = 0; i < 5; i++) {
            if (i < static_cast<int>(highScores.size())) {
                std::string text = std::to_string(i + 1) + ". " +
                                   highScores[i].name + " - " +
                                   std::to_string(highScores[i].score);
                highScoresText[i].setString(text);
            } else {
                std::string text = std::to_string(i + 1) + ". --- - 0";
                highScoresText[i].setString(text);
            }

            // Re-center after text change
            sf::FloatRect scoreBounds = highScoresText[i].getLocalBounds();
            highScoresText[i].setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            highScoresText[i].setPosition(window->getSize().x / 2.0f, 330 + i * 50);
        }
    }
}
