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
            : State(win, fac, cam, sm), mapFile(mapFile) {

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

            livesText.setFont(font);
            livesText.setCharacterSize(24);
            livesText.setFillColor(sf::Color::White);
            livesText.setPosition(10.0f, 40.0f);
        } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            fontLoaded = true;

            scoreText.setFont(font);
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::Yellow);
            scoreText.setPosition(10.0f, 10.0f);

            livesText.setFont(font);
            livesText.setCharacterSize(24);
            livesText.setFillColor(sf::Color::White);
            livesText.setPosition(10.0f, 40.0f);
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

            logic::PacManModel* pacman = world->getPacMan();
            if (pacman) {
                livesText.setString("LIVES: " + std::to_string(pacman->getLives()));
            }
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
            window->draw(livesText);

            // ← ADD: Show current level
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

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P) {
                stateManager->pushState(std::make_unique<PausedState>(
                        window, factory, camera, stateManager, this, mapFile
                ));
                return;
            }

            if (event.key.code == sf::Keyboard::Escape) {
                window->close();
                return;
            }

            if (event.key.code == sf::Keyboard::N) {
                world->nextLevel();
                isCountingDown = true;
                countdownTimer = 2.0f;
                std::cout << "DEBUG: Skipped to level " << world->getCurrentLevel() << std::endl;
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
