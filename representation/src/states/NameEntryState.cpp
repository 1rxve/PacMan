#include "representation/states/NameEntryState.h"
#include "logic/utils/Score.h"
#include "representation/StateManager.h"
#include "representation/states/VictoryState.h"

namespace representation {
NameEntryState::NameEntryState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                               int finalScore, const std::string& mapFile)
    : State(win, fac, cam, sm), finalScore(finalScore), mapFile(mapFile), playerName("   "), currentLetterIndex(0),
      fontLoaded(false), blinkTimer(0.0f), cursorVisible(true), isNewHighScore(logic::Score::isTopScore(finalScore)) {

    if (font.loadFromFile("resources/fonts/joystix.otf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }

    if (fontLoaded) {
        titleText.setFont(font);
        if (isNewHighScore) {
            titleText.setString("NEW HIGH SCORE!");
        } else {
            titleText.setString("HIGH SCORE");
        }
        titleText.setCharacterSize(64);
        titleText.setFillColor(sf::Color::Yellow);

        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
        titleText.setPosition(window->getSize().x / 2.0f, 150);

        scoreText.setFont(font);
        scoreText.setString("SCORE: " + std::to_string(finalScore));
        scoreText.setCharacterSize(32);
        scoreText.setFillColor(sf::Color::White);

        sf::FloatRect scoreBounds = scoreText.getLocalBounds();
        scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
        scoreText.setPosition(window->getSize().x / 2.0f, 300);

        nameText.setFont(font);
        nameText.setCharacterSize(80);
        nameText.setFillColor(sf::Color::White);

        instructionText.setFont(font);
        instructionText.setString("ENTER NAME");
        instructionText.setCharacterSize(32);
        instructionText.setFillColor(sf::Color::White);

        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
        instructionText.setPosition(window->getSize().x / 2.0f, 350);

        pressEnterText.setFont(font);
        pressEnterText.setString("PRESS \"ENTER\"");
        pressEnterText.setCharacterSize(24);
        pressEnterText.setFillColor(sf::Color::Yellow);

        sf::FloatRect pressEnterBounds = pressEnterText.getLocalBounds();
        pressEnterText.setOrigin(pressEnterBounds.width / 2.0f, pressEnterBounds.height / 2.0f);
        pressEnterText.setPosition(window->getSize().x / 2.0f, 800);

        whenDoneText.setFont(font);
        whenDoneText.setString("WHEN YOU ARE DONE");
        whenDoneText.setCharacterSize(24);
        whenDoneText.setFillColor(sf::Color::Yellow);

        sf::FloatRect whenDoneBounds = whenDoneText.getLocalBounds();
        whenDoneText.setOrigin(whenDoneBounds.width / 2.0f, whenDoneBounds.height / 2.0f);
        whenDoneText.setPosition(window->getSize().x / 2.0f, 840);

        updateNameDisplay();
    }
}

void NameEntryState::updateNameDisplay() {
    const float LETTER_WIDTH = 80.0f;  // Letter cell width in pixels
    const float SPACING = 20.0f;        // Horizontal spacing between letters
    float startX = window->getSize().x / 2.0f - (3 * LETTER_WIDTH + 2 * SPACING) / 2.0f;

    float cursorX = startX + currentLetterIndex * (LETTER_WIDTH + SPACING) + LETTER_WIDTH / 2.0f;
    cursorText.setPosition(cursorX - 10, 410);
}

void NameEntryState::saveAndContinue() {
    logic::Score::saveHighScore(playerName, finalScore);

    // CRITICAL: Copy ALL needed data to LOCAL variables BEFORE popping
    StateManager* sm = stateManager;
    sf::RenderWindow* win = window;
    logic::AbstractFactory* fac = factory;
    const Camera* cam = camera;
    int score = finalScore;
    std::string map = mapFile;

    sm->popState(); // Pop NameEntryState - 'this' is now DESTROYED
    // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

    sm->pushState(std::make_unique<VictoryState>(win, fac, cam, sm, true, score, map));
}

void NameEntryState::update(float deltaTime) {
    blinkTimer += deltaTime;
    if (blinkTimer >= 0.5f) {
        cursorVisible = !cursorVisible;
        blinkTimer = 0.0f;
    }
}

void NameEntryState::render() {
    // Semi-transparent dark overlay (alpha 220 = ~86% opacity)
    sf::RectangleShape overlay(
        sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(window->getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 220));
    window->draw(overlay);

    if (fontLoaded) {
        window->draw(titleText);
        window->draw(scoreText);
        window->draw(instructionText);

        const float LETTER_WIDTH = 80.0f;
        const float SPACING = 20.0f;
        float startX = window->getSize().x / 2.0f - (3 * LETTER_WIDTH + 2 * SPACING) / 2.0f;
        float startY = 500.0f;

        for (int i = 0; i < 3; i++) {
            float letterX = startX + i * (LETTER_WIDTH + SPACING);

            sf::Text letterText;
            letterText.setFont(font);

            // Display underscore for empty positions, actual letter otherwise
            if (playerName[i] != ' ') {
                letterText.setString(std::string(1, playerName[i]));
            } else {
                letterText.setString("_");
            }

            letterText.setCharacterSize(80);
            letterText.setFillColor(sf::Color::White);

            sf::FloatRect letterBounds = letterText.getLocalBounds();
            letterText.setOrigin(letterBounds.width / 2.0f, letterBounds.height / 2.0f);
            letterText.setPosition(letterX + LETTER_WIDTH / 2.0f, startY);

            window->draw(letterText);
        }

        window->draw(pressEnterText);
        window->draw(whenDoneText);
    }
}

void NameEntryState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::TextEntered) {
        char typed = static_cast<char>(event.text.unicode);

        if (typed >= 'a' && typed <= 'z') {
            typed = typed - 'a' + 'A';
        }

        if (typed >= 'A' && typed <= 'Z') {
            playerName[currentLetterIndex] = typed;

            // Auto-advance to next position (if not at end)
            if (currentLetterIndex < 2) {
                currentLetterIndex++;
            }

            updateNameDisplay();
        }

        return;
    }

    if (event.type != sf::Event::KeyPressed) {
        return;
    }

    if (event.key.code == sf::Keyboard::Backspace) {
        playerName[currentLetterIndex] = ' ';

        if (currentLetterIndex > 0) {
            currentLetterIndex--;
        }

        updateNameDisplay();
    }
    else if (event.key.code == sf::Keyboard::Enter) {
        bool hasLetter = false;
        for (int i = 0; i < 3; i++) {
            if (playerName[i] != ' ') {
                hasLetter = true;
                break;
            }
        }

        if (hasLetter) {
            // Auto-fill trailing spaces with 'A' for valid 3-letter name
            for (int i = 0; i < 3; i++) {
                if (playerName[i] == ' ') {
                    playerName[i] = 'A';
                }
            }
            saveAndContinue();
        }
    }
}
} // namespace representation