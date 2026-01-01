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
              playerName("   "),  // ← 3 spaties (leeg)
              currentLetterIndex(0),
              fontLoaded(false),
              blinkTimer(0.0f),
              cursorVisible(true),
              isNewHighScore(logic::Score::isTopScore(finalScore)) {

        // Load font
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;
        }

        if (fontLoaded) {
            // Title - Yellow like MenuState
            titleText.setFont(font);
            if (isNewHighScore) {
                titleText.setString("NEW HIGH SCORE!");
            } else {
                titleText.setString("HIGH SCORE");
            }
            titleText.setCharacterSize(64);  // ← Bigger
            titleText.setFillColor(sf::Color::Yellow);  // ← Match menu

            sf::FloatRect titleBounds = titleText.getLocalBounds();
            titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
            titleText.setPosition(window->getSize().x / 2.0f, 150);

            // Score - White
            scoreText.setFont(font);
            scoreText.setString("SCORE: " + std::to_string(finalScore));
            scoreText.setCharacterSize(32);
            scoreText.setFillColor(sf::Color::White);  // ← Keep white

            sf::FloatRect scoreBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(scoreBounds.width / 2.0f, scoreBounds.height / 2.0f);
            scoreText.setPosition(window->getSize().x / 2.0f, 300);

            // Name entry text - White (will be drawn as letters in render)
            nameText.setFont(font);
            nameText.setCharacterSize(80);
            nameText.setFillColor(sf::Color::White);  // ← CHANGE from Cyan

            // Instructions - White
            instructionText.setFont(font);
            instructionText.setString("ENTER NAME");
            instructionText.setCharacterSize(32);
            instructionText.setFillColor(sf::Color::White);  // ← CHANGE from Cyan

            sf::FloatRect instrBounds = instructionText.getLocalBounds();
            instructionText.setOrigin(instrBounds.width / 2.0f, instrBounds.height / 2.0f);
            instructionText.setPosition(window->getSize().x / 2.0f, 350);

            pressEnterText.setFont(font);
            pressEnterText.setString("PRESS \"ENTER\"");
            pressEnterText.setCharacterSize(24);
            pressEnterText.setFillColor(sf::Color::Yellow);  // ← CHANGE

            sf::FloatRect pressEnterBounds = pressEnterText.getLocalBounds();
            pressEnterText.setOrigin(pressEnterBounds.width / 2.0f, pressEnterBounds.height / 2.0f);
            pressEnterText.setPosition(window->getSize().x / 2.0f, 800);

            // "WHEN YOU ARE DONE" text
            whenDoneText.setFont(font);
            whenDoneText.setString("WHEN YOU ARE DONE");
            whenDoneText.setCharacterSize(24);
            whenDoneText.setFillColor(sf::Color::Yellow);  // ← CHANGE

            sf::FloatRect whenDoneBounds = whenDoneText.getLocalBounds();
            whenDoneText.setOrigin(whenDoneBounds.width / 2.0f, whenDoneBounds.height / 2.0f);
            whenDoneText.setPosition(window->getSize().x / 2.0f, 840);

            updateNameDisplay();
        }
    }

    void NameEntryState::updateNameDisplay() {
        // Just update cursor position
        float letterWidth = 80.0f;
        float spacing = 20.0f;
        float startX = window->getSize().x / 2.0f - (3 * letterWidth + 2 * spacing) / 2.0f;

        float cursorX = startX + currentLetterIndex * (letterWidth + spacing) + letterWidth / 2.0f;
        cursorText.setPosition(cursorX - 10, 410);
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
            window->draw(instructionText);

            // Draw 3 letters WITHOUT boxes
            float letterWidth = 80.0f;
            float spacing = 20.0f;
            float startX = window->getSize().x / 2.0f - (3 * letterWidth + 2 * spacing) / 2.0f;
            float startY = 500.0f;

            for (int i = 0; i < 3; i++) {
                float letterX = startX + i * (letterWidth + spacing);

                // Draw letter (or space)
                sf::Text letterText;
                letterText.setFont(font);

                if (playerName[i] != ' ') {
                    letterText.setString(std::string(1, playerName[i]));
                } else {
                    letterText.setString("_");  // Show underscore for empty
                }

                letterText.setCharacterSize(80);
                letterText.setFillColor(sf::Color::White);

                sf::FloatRect letterBounds = letterText.getLocalBounds();
                letterText.setOrigin(letterBounds.width / 2.0f, letterBounds.height / 2.0f);
                letterText.setPosition(letterX + letterWidth / 2.0f, startY);

                window->draw(letterText);
            }

            // NO CURSOR DRAWN - REMOVED

            window->draw(pressEnterText);
            window->draw(whenDoneText);
        }
    }

    void NameEntryState::handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::TextEntered) {
            char typed = static_cast<char>(event.text.unicode);

            // Convert lowercase to uppercase
            if (typed >= 'a' && typed <= 'z') {
                typed = typed - 'a' + 'A';
            }

            // Only accept A-Z
            if (typed >= 'A' && typed <= 'Z') {
                playerName[currentLetterIndex] = typed;

                // Auto-advance to next empty box (if not at end)
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

        // Backspace - delete current letter and move back
        if (event.key.code == sf::Keyboard::Backspace) {
            // Clear current letter
            playerName[currentLetterIndex] = ' ';

            // Move back if not at start
            if (currentLetterIndex > 0) {
                currentLetterIndex--;
            }

            updateNameDisplay();
        }
            // Enter - save and continue (only if at least 1 letter entered)
        else if (event.key.code == sf::Keyboard::Enter) {
            // Check if at least one letter is entered
            bool hasLetter = false;
            for (int i = 0; i < 3; i++) {
                if (playerName[i] != ' ') {
                    hasLetter = true;
                    break;
                }
            }

            if (hasLetter) {
                // Replace trailing spaces with 'A' for valid name
                for (int i = 0; i < 3; i++) {
                    if (playerName[i] == ' ') {
                        playerName[i] = 'A';
                    }
                }
                saveAndContinue();
            }
        }
    }
}