#include "representation/states/NameEntryState.h"
#include "representation/states/VictoryState.h"
#include "representation/StateManager.h"
#include "logic/utils/Score.h"

namespace representation {
    NameEntryState::NameEntryState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                                   const Camera* cam, StateManager* sm,
                                   int finalScore, const std::string& mapFile)
            : State(win, fac, cam, sm),
              finalScore(finalScore),
              mapFile(mapFile),
              playerName("AAA"),
              currentLetterIndex(0),
              fontLoaded(false),
              blinkTimer(0.0f),
              cursorVisible(true) {

        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Title
            titleText.setFont(font);
            titleText.setString("NEW HIGH SCORE!");
            titleText.setCharacterSize(48);
            titleText.setFillColor(sf::Color::Yellow);

            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, 150);

            // Score
            scoreText.setFont(font);
            scoreText.setString("SCORE: " + std::to_string(finalScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color::White);

            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            scoreText.setPosition(window->getSize().x / 2.0f, 250);

            // Name entry
            nameText.setFont(font);
            nameText.setCharacterSize(80);
            nameText.setFillColor(sf::Color::Cyan);
            nameText.setPosition(window->getSize().x / 2.0f - 120, 350);

            // Cursor (underscore under current letter)
            cursorText.setFont(font);
            cursorText.setString("_");
            cursorText.setCharacterSize(80);
            cursorText.setFillColor(sf::Color::Yellow);

            // Instructions
            instructionText.setFont(font);
            instructionText.setString("Type your name (3 letters)\nENTER: Confirm  BACKSPACE: Delete");            instructionText.setCharacterSize(24);
            instructionText.setFillColor(sf::Color::White);

            sf::FloatRect instrBounds = instructionText.getLocalBounds();
            instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
            instructionText.setPosition(window->getSize().x / 2.0f, 550);

            updateNameDisplay();
        }
    }

    void NameEntryState::updateNameDisplay() {
        nameText.setString(playerName);

        // Position cursor under current letter
        float letterWidth = 80.0f * 0.6f;  // Approximate monospace width
        float cursorX = window->getSize().x / 2.0f - 120 + currentLetterIndex * letterWidth;
        cursorText.setPosition(cursorX, 420);
    }

    void NameEntryState::incrementLetter() {
        char& currentChar = playerName[currentLetterIndex];
        if (currentChar == 'Z') {
            currentChar = 'A';
        } else {
            currentChar++;
        }
        updateNameDisplay();
    }

    void NameEntryState::decrementLetter() {
        char& currentChar = playerName[currentLetterIndex];
        if (currentChar == 'A') {
            currentChar = 'Z';
        } else {
            currentChar--;
        }
        updateNameDisplay();
    }

    void NameEntryState::saveAndContinue() {
        // Save high score
        logic::Score::saveHighScore(playerName, finalScore);

        // CRITICAL: Copy ALL needed data to LOCAL variables BEFORE popping
        StateManager* sm = stateManager;
        sf::RenderWindow* win = window;
        logic::AbstractFactory* fac = factory;
        const Camera* cam = camera;
        int score = finalScore;
        std::string map = mapFile;

        sm->popState();  // Pop NameEntryState - 'this' is now DESTROYED
        // DO NOT ACCESS ANY MEMBER VARIABLES AFTER THIS LINE

        // Push VictoryState with LOCAL copies
        sm->pushState(std::make_unique<VictoryState>(
                win, fac, cam, sm,
                true,
                score,
                map
        ));
    }

    void NameEntryState::update(float deltaTime) {
        // Blink cursor
        blinkTimer += deltaTime;
        if (blinkTimer >= 0.5f) {
            cursorVisible = !cursorVisible;
            blinkTimer = 0.0f;
        }
    }

    void NameEntryState::render() {
        // Dark overlay
        sf::RectangleShape overlay(sf::Vector2f(
                static_cast<float>(window->getSize().x),
                static_cast<float>(window->getSize().y)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 220));
        window->draw(overlay);

        if (fontLoaded) {
            window->draw(titleText);
            window->draw(scoreText);
            window->draw(nameText);

            if (cursorVisible) {
                window->draw(cursorText);
            }

            window->draw(instructionText);
        }
    }

    void NameEntryState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::TextEntered) {
            // Handle typed characters
            char typed = static_cast<char>(event.text.unicode);

            // Only accept A-Z (uppercase)
            if (typed >= 'a' && typed <= 'z') {
                typed = typed - 'a' + 'A';  // Convert to uppercase
            }

            if (typed >= 'A' && typed <= 'Z') {
                playerName[currentLetterIndex] = typed;

                // Auto-advance to next letter (max 2)
                if (currentLetterIndex < 2) {
                    currentLetterIndex++;
                }

                updateNameDisplay();
            }
        }

        if (event.type != sf::Event::KeyPressed) {
            return;
        }

        // Backspace
        if (event.key.code == sf::Keyboard::Backspace) {
            if (currentLetterIndex > 0) {
                currentLetterIndex--;
                playerName[currentLetterIndex] = 'A';
                updateNameDisplay();
            }
        }
            // Left arrow
        else if (event.key.code == sf::Keyboard::Left) {
            currentLetterIndex--;
            if (currentLetterIndex < 0) {
                currentLetterIndex = 0;
            }
            updateNameDisplay();
        }
            // Right arrow
        else if (event.key.code == sf::Keyboard::Right) {
            currentLetterIndex++;
            if (currentLetterIndex > 2) {
                currentLetterIndex = 2;
            }
            updateNameDisplay();
        }
            // Enter/Space to confirm
        else if (event.key.code == sf::Keyboard::Enter ||
                 event.key.code == sf::Keyboard::Space) {
            saveAndContinue();
        }
            // Escape to skip
        else if (event.key.code == sf::Keyboard::Escape) {
            saveAndContinue();
        }
    }
}