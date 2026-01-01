#include "representation/states/LevelState.h"
#include "representation/Camera.h"
#include "representation/states/PausedState.h"
#include "representation/states/VictoryState.h"
#include "representation/StateManager.h"
#include "logic/entities/PacManModel.h"
#include "representation/states/NameEntryState.h"


namespace representation {
    LevelState::~LevelState() {
        // World unique_ptr auto-destructs and handles cleanup
    }

    LevelState::LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac,
                           const Camera* cam, StateManager* sm,
                           const std::string& mapFile)
            : State(win, fac, cam, sm), mapFile(mapFile), cheatBuffer("") {

        // Create World
        world = std::make_unique<logic::World>();
        world->setFactory(factory);
        world->loadMap(mapFile);

        // Load font for UI
        fontLoaded = false;
        if (font.loadFromFile("resources/fonts/joystix.otf")) {
            fontLoaded = true;

            scoreText.setFont(font);
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::Yellow);
            scoreText.setPosition(10.0f, 10.0f);

            // Lives icons - load Pac-Man sprite
            livesTexture = std::make_shared<sf::Texture>();
            if (livesTexture->loadFromFile("resources/sprites/pacman_sprites.png")) {
                livesSprite.setTexture(*livesTexture);
                livesSprite.setTextureRect(sf::IntRect(840, 0, 50, 50));  // Full circle
                livesSprite.setOrigin(25.0f, 25.0f);
                livesSprite.setScale(0.6f, 0.6f);  // Small icons
            }
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;

            scoreText.setFont(font);
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::Yellow);
            scoreText.setPosition(10.0f, 10.0f);

            // Lives icons - load Pac-Man sprite
            livesTexture = std::make_shared<sf::Texture>();
            if (livesTexture->loadFromFile("resources/sprites/pacman_sprites.png")) {
                livesSprite.setTexture(*livesTexture);
                livesSprite.setTextureRect(sf::IntRect(840, 0, 50, 50));  // Full circle
                livesSprite.setOrigin(25.0f, 25.0f);
                livesSprite.setScale(0.6f, 0.6f);  // Small icons
            }
        }

        // Setup PacMan cell dimensions
        logic::PacManModel* pacman = world->getPacMan();
        if (pacman) {
            auto [mapWidth, mapHeight] = logic::World::getMapDimensions(mapFile);
            float cellWidth = 2.0f / mapWidth;
            float cellHeight = 2.0f / mapHeight;
            pacman->setCellDimensions(cellWidth, cellHeight);
        }

        world->getScoreSubject()->attach(world->getScoreObject());

        isCountingDown = true;
        countdownTimer = 1.0f;

        // ← ADD: Setup "READY!" text
        if (fontLoaded) {
            readyText.setFont(font);
            readyText.setString("READY!");
            readyText.setCharacterSize(35);
            readyText.setFillColor(sf::Color::Yellow);

            sf::FloatRect bounds = readyText.getLocalBounds();
            readyText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            readyText.setPosition(window->getSize().x / 2.0f, window->getSize().y / 2.0f - 7);
        }
    }

    void LevelState::update(float deltaTime) {
        // Countdown logic
        if (isCountingDown) {
            countdownTimer -= deltaTime;

            if (countdownTimer <= 0.0f) {
                isCountingDown = false;
                world->clearRespawnFlag();  // ← ADD
            }


            return;
        }

        // ← ADD: Check if just respawned (before world update)
        if (world->justRespawned()) {
            isCountingDown = true;
            countdownTimer = 1.0f;
            return;
        }

        world->update(deltaTime);

        // Update UI text
        if (fontLoaded) {
            scoreText.setString("SCORE: " + std::to_string(world->getScore()));
            // Lives are rendered as icons, no text update needed
        }

        // Victory conditions
        int coinsCollected = world->getCoinsCollected();
        int totalCoins = world->getTotalCoins();

        // WIN: All coins collected
        if (coinsCollected >= totalCoins) {

            world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
            world->getScoreSubject()->notify();

            // Show brief "LEVEL X" message (optional)
            std::cout << "Level cleared! Starting level " << world->getCurrentLevel() << std::endl;

            // Trigger countdown
            isCountingDown = true;
            countdownTimer = 2.0f;

            return;
        }

        // LOSE: No lives remaining
        logic::PacManModel* pacman = world->getPacMan();
        // GAME OVER: No lives remaining
        if (pacman && pacman->getLives() <= 0) {
            int finalScore = world->getScore();

            // Check if top 5 high score
            if (logic::Score::isHighScore(finalScore)) {
                // Name entry → Game Over screen
                stateManager->pushState(std::make_unique<NameEntryState>(
                        window, factory, camera, stateManager,
                        finalScore,
                        mapFile
                ));
            } else {
                // Directly to Game Over screen
                stateManager->pushState(std::make_unique<VictoryState>(
                        window, factory, camera, stateManager,
                        false,  // won = false (game over)
                        finalScore,
                        mapFile
                ));
            }
            return;
        }
    }

    void LevelState::render() {

        // ← ADD THIS: Render all game entities
        world->renderInOrder();

        // Draw "READY!" during countdown
        if (isCountingDown && fontLoaded) {
            window->draw(readyText);
        }

        // Draw UI overlay
        if (fontLoaded) {
            window->draw(scoreText);

            // Draw lives as Pac-Man icons
            logic::PacManModel* pacman = world->getPacMan();
            if (pacman && livesTexture) {
                int lives = pacman->getLives();
                for (int i = 0; i < lives; i++) {
                    livesSprite.setPosition(20.0f + i * 35.0f, 55.0f);  // Horizontal spacing
                    window->draw(livesSprite);
                }
            }

            // Show current level
            sf::Text levelText;
            levelText.setFont(font);
            levelText.setString("LEVEL: " + std::to_string(world->getCurrentLevel()));
            levelText.setCharacterSize(24);
            levelText.setFillColor(sf::Color::White);
            levelText.setPosition(10.0f, 70.0f);
            window->draw(levelText);
        }
    }

    void LevelState::handleEvent(const sf::Event& event) {
        logic::PacManModel* pacman = world->getPacMan();
        if (!pacman) return;

        // ========== CHEAT CODE HANDLING ==========
        if (event.type == sf::Event::TextEntered) {
            char typed = static_cast<char>(event.text.unicode);

            // Only accept A-Z letters (case insensitive)
            if ((typed >= 'a' && typed <= 'z') || (typed >= 'A' && typed <= 'Z')) {
                // Convert to uppercase
                if (typed >= 'a' && typed <= 'z') {
                    typed = typed - 'a' + 'A';
                }

                cheatBuffer += typed;

                // Keep buffer max 10 chars (prevents memory issues)
                if (cheatBuffer.length() > 10) {
                    cheatBuffer = cheatBuffer.substr(1);  // Remove first char
                }

                // Check for cheat codes
                if (cheatBuffer.find("GHOST") != std::string::npos) {
                    world->activateFearMode();
                    std::cout << "CHEAT: GHOST activated - Fear mode!" << std::endl;
                    cheatBuffer.clear();  // Clear buffer after activation
                }

                if (cheatBuffer.find("LEVEL") != std::string::npos) {
                    // Award level clear bonus BEFORE nextLevel (which resets coins)
                    world->getScoreSubject()->notify();  // Trigger score update
                    world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
                    world->getScoreSubject()->notify();

                    world->nextLevel();
                    isCountingDown = true;
                    countdownTimer = 2.0f;
                    std::cout << "CHEAT: LEVEL activated - Skip to level "
                              << world->getCurrentLevel() << " (+500 bonus)" << std::endl;
                    cheatBuffer.clear();
                }
            }
        }
        // ========================================

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P) {
                cheatBuffer.clear();  // ← ADD: Clear buffer bij pause
                stateManager->pushState(std::make_unique<PausedState>(
                        window, factory, camera, stateManager, this, mapFile
                ));
                return;
            }

            if (event.key.code == sf::Keyboard::Escape) {
                cheatBuffer.clear();  // ← ADD
                window->close();
                return;
            }

            // Arrow key handling
            if (event.key.code == sf::Keyboard::Up) {
                if (pacman->getCurrentDirection() != logic::Direction::UP) {
                    pacman->setNextDirection(logic::Direction::UP);
                }
            } else if (event.key.code == sf::Keyboard::Down) {
                if (pacman->getCurrentDirection() != logic::Direction::DOWN) {
                    pacman->setNextDirection(logic::Direction::DOWN);
                }
            } else if (event.key.code == sf::Keyboard::Left) {
                if (pacman->getCurrentDirection() != logic::Direction::LEFT) {
                    pacman->setNextDirection(logic::Direction::LEFT);
                }
            } else if (event.key.code == sf::Keyboard::Right) {
                if (pacman->getCurrentDirection() != logic::Direction::RIGHT) {
                    pacman->setNextDirection(logic::Direction::RIGHT);
                }
            }
        }
    }
}
