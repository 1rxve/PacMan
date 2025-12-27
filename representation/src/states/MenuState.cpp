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
            titleText.setPosition(window->getSize().x / 2.0f - 200, 50);

            // High scores title
            highScoresTitle.setFont(font);
            highScoresTitle.setString("HIGH SCORES");
            highScoresTitle.setCharacterSize(40);
            highScoresTitle.setFillColor(sf::Color::Cyan);
            highScoresTitle.setPosition(window->getSize().x / 2.0f - 150, 200);

            // Top 5 high scores (placeholder data for now)
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
                highScoresText[i].setPosition(window->getSize().x / 2.0f - 120, 280 + i * 50);
            }

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press SPACE to start");
            instructionText.setCharacterSize(30);
            instructionText.setFillColor(sf::Color::Green);
            instructionText.setPosition(window->getSize().x / 2.0f - 180, 600);
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
        }
    }
}
