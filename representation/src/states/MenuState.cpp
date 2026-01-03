#include "representation/states/MenuState.h"
#include "logic/utils/Score.h"
#include "representation/StateManager.h"
#include "representation/states/LevelState.h"

namespace representation {
MenuState::MenuState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                     const std::string& mapFile)
    : State(win, fac, cam, sm), mapFile(mapFile), fontLoaded(false), blinkTimer(0.0f), instructionVisible(true) {

    SoundManager::getInstance().playMenuMusic();

    // Load custom font with fallback
    if (font.loadFromFile("resources/fonts/joystix.otf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }

    if (fontLoaded) {
        titleText.setFont(font);
        titleText.setString("PAC-MAN");
        titleText.setCharacterSize(140);
        titleText.setFillColor(sf::Color::Yellow);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
        titleText.setPosition(window->getSize().x / 2.0f, 120);

        highScoresTitle.setFont(font);
        highScoresTitle.setString("HIGH SCORES");
        highScoresTitle.setCharacterSize(32);
        highScoresTitle.setFillColor(sf::Color::White);

        sf::FloatRect hsBounds = highScoresTitle.getLocalBounds();
        highScoresTitle.setOrigin(hsBounds.width / 2.0f, hsBounds.height / 2.0f);
        highScoresTitle.setPosition(window->getSize().x / 2.0f, 350);

        auto highScores = logic::Score::loadHighScores();

        for (int i = 0; i < 5; i++) {
            highScoresText[i].setFont(font);

            if (i < static_cast<int>(highScores.size())) {
                std::string text =
                    std::to_string(i + 1) + ". " + highScores[i].name + " - " + std::to_string(highScores[i].score);
                highScoresText[i].setString(text);
            } else {
                std::string text = std::to_string(i + 1) + ". --- - 0";
                highScoresText[i].setString(text);
            }

            highScoresText[i].setCharacterSize(26);
            highScoresText[i].setFillColor(sf::Color::White);

            sf::FloatRect scoreBounds = highScoresText[i].getLocalBounds();
            highScoresText[i].setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            highScoresText[i].setPosition(window->getSize().x / 2.0f, 450 + i * 50);
        }

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

MenuState::~MenuState() {}

void MenuState::update(float deltaTime) {
    // Blink instruction text every 0.5 seconds for visual feedback
    blinkTimer += deltaTime;

    if (blinkTimer >= 0.5f) {
        instructionVisible = !instructionVisible;
        blinkTimer = 0.0f;
    }

    // Refresh high scores on second update() call (after returning from gameplay)
    // First call sets flag, second call refreshes, prevents double-refresh
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

        for (int i = 0; i < 5; i++) {
            window->draw(highScoresText[i]);
        }

        if (instructionVisible) {
            window->draw(instructionText);
        }
    } else {
        // Fallback for missing fonts (development/debugging aid)
        sf::RectangleShape titleBox(sf::Vector2f(300, 100));
        titleBox.setFillColor(sf::Color::Yellow);
        titleBox.setPosition(window->getSize().x / 2.0f - 150, 50);
        window->draw(titleBox);
    }
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            stateManager->pushState(std::make_unique<LevelState>(window, factory, camera, stateManager, mapFile));
        }

        if (event.key.code == sf::Keyboard::Escape) {
            window->close();
        }
    }
}

void MenuState::refreshHighScores() {
    if (!fontLoaded)
        return;

    auto highScores = logic::Score::loadHighScores();

    for (int i = 0; i < 5; i++) {
        if (i < static_cast<int>(highScores.size())) {
            std::string text =
                std::to_string(i + 1) + ". " + highScores[i].name + " - " + std::to_string(highScores[i].score);
            highScoresText[i].setString(text);
        } else {
            std::string text = std::to_string(i + 1) + ". --- - 0";
            highScoresText[i].setString(text);
        }

        // Re-center text after content change (text width varies with score)
        sf::FloatRect scoreBounds = highScoresText[i].getLocalBounds();
        highScoresText[i].setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
        highScoresText[i].setPosition(window->getSize().x / 2.0f, 450 + i * 50);
    }
}
} // namespace representation