#include "representation/states/MenuState.h"
#include "representation/states/LevelState.h"
#include "representation/StateManager.h"
#include "logic/utils/Score.h"

namespace representation {
    MenuState::MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                         const Camera* cam, StateManager* sm,
                         const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile), fontLoaded(false),
              blinkTimer(0.0f), instructionVisible(true) {

        SoundManager::getInstance().playMenuMusic();

        // Load custom font with fallback
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // PAC-MAN title - LARGE, no outline
            titleText.setFont(font);
            titleText.setString("PAC-MAN");
            titleText.setCharacterSize(140);
            titleText.setFillColor(sf::Color::Yellow);
            // NO outline

            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, 120);

            // HIGH SCORES title - White
            highScoresTitle.setFont(font);
            highScoresTitle.setString("HIGH SCORES");
            highScoresTitle.setCharacterSize(32);
            highScoresTitle.setFillColor(sf::Color::White);

            sf::FloatRect hsBounds = highScoresTitle.getLocalBounds();
            highScoresTitle.setOrigin(hsBounds.width / 2.0f, hsBounds.height / 2.0f);
            highScoresTitle.setPosition(window->getSize().x / 2.0f, 350);  // ← FIX

            // Top 5 high scores
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

                highScoresText[i].setCharacterSize(26);
                highScoresText[i].setFillColor(sf::Color::White);

                sf::FloatRect scoreBounds = highScoresText[i].getLocalBounds();
                highScoresText[i].setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
                highScoresText[i].setPosition(window->getSize().x / 2.0f, 450 + i * 50);  // ← FIX
            }

            // Instruction text
            instructionText.setFont(font);
            instructionText.setString("Press SPACE to start");
            instructionText.setCharacterSize(28);
            instructionText.setFillColor(sf::Color::Yellow);

            sf::FloatRect instrBounds = instructionText.getLocalBounds();
            instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
            instructionText.setPosition(window->getSize().x / 2.0f, 800);
        }

        needsRefresh = false;
    }

    MenuState::~MenuState() {
    }

    void MenuState::update(float deltaTime) {
        blinkTimer += deltaTime;

        if (blinkTimer >= 0.5f) {
            instructionVisible = !instructionVisible;
            blinkTimer = 0.0f;
        }

        // Restart menu music if it stopped playing
        if (SoundManager::getInstance().isMenuMusicPlaying() == false) {
            SoundManager::getInstance().playMenuMusic();
        }

        if (!needsRefresh) {
            needsRefresh = true;
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

            // Draw blinking instruction text
            if (instructionVisible) {
                window->draw(instructionText);
            }
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
            highScoresText[i].setPosition(window->getSize().x / 2.0f, 450 + i * 50);
        }
    }
}
