#include "representation/states/LevelState.h"
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"
#include "representation/SoundManager.h"
#include "representation/StateManager.h"
#include "representation/states/NameEntryState.h"
#include "representation/states/PausedState.h"
#include "representation/states/VictoryState.h"

namespace representation {
LevelState::~LevelState() {
    // World unique_ptr auto-destructs and handles cleanup
}

LevelState::LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
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
            livesSprite.setTextureRect(sf::IntRect(840, 0, 50, 50)); // Full circle
            livesSprite.setOrigin(25.0f, 25.0f);
            livesSprite.setScale(0.6f, 0.6f); // Small icons
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
            livesSprite.setTextureRect(sf::IntRect(840, 0, 50, 50)); // Full circle
            livesSprite.setOrigin(25.0f, 25.0f);
            livesSprite.setScale(0.6f, 0.6f); // Small icons
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

    SoundManager::getInstance().stopMenuMusic();

    soundObserver = std::make_unique<SoundObserver>(world->getScoreObject());

    world->getScoreSubject()->attach(world->getScoreObject());
    world->getScoreSubject()->attach(soundObserver.get());

    isCountingDown = true;
    countdownTimer = 1.0f;

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

// In LevelState::update()
void LevelState::update(float deltaTime) {
    // Countdown logic
    if (isCountingDown) {
        countdownTimer -= deltaTime;

        if (countdownTimer <= 0.0f) {
            isCountingDown = false;
            world->clearRespawnFlag();
        }
        return;
    }

    SoundManager::getInstance().update(deltaTime);

    // Check if just respawned (before world update)
    if (world->justRespawned()) {
        isCountingDown = true;
        countdownTimer = 1.0f;
        SoundManager::getInstance().stopCoinSound();
        return;
    }

    world->update(deltaTime);

    // Update UI text
    if (fontLoaded) {
        scoreText.setString("SCORE: " + std::to_string(world->getScore()));
    }

    // Victory conditions
    int coinsCollected = world->getCoinsCollected();
    int totalCoins = world->getTotalCoins();

    // WIN: All coins collected
    if (coinsCollected >= totalCoins) {
        world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
        world->getScoreSubject()->notify();

        SoundManager::getInstance().stopCoinSound();

        // ← CHANGE: Call nextLevel() IMMEDIATELY
        world->nextLevel();

        // ← THEN start countdown
        isCountingDown = true;
        countdownTimer = 2.0f;

        return;
    }

    // LOSE: No lives remaining
    logic::PacManModel* pacman = world->getPacMan();
    if (pacman && pacman->getLives() <= 0) {
        int finalScore = world->getScore();

        if (logic::Score::isHighScore(finalScore)) {
            stateManager->pushState(
                std::make_unique<NameEntryState>(window, factory, camera, stateManager, finalScore, mapFile));
        } else {
            stateManager->pushState(
                std::make_unique<VictoryState>(window, factory, camera, stateManager, false, finalScore, mapFile));
        }
        return;
    }
}

void LevelState::render() {

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
                livesSprite.setPosition(20.0f + i * 35.0f, 55.0f); // Horizontal spacing
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
    if (!pacman)
        return;

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
                cheatBuffer = cheatBuffer.substr(1); // Remove first char
            }

            // Check for cheat codes
            if (cheatBuffer.find("GHOST") != std::string::npos) {
                world->activateFearMode();
                cheatBuffer.clear(); // Clear buffer after activation
            }

            if (cheatBuffer.find("LEVEL") != std::string::npos) {
                // Award level clear bonus BEFORE nextLevel (which resets coins)
                world->getScoreSubject()->notify(); // Trigger score update
                world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
                world->getScoreSubject()->notify();

                world->nextLevel();
                isCountingDown = true;
                countdownTimer = 2.0f;
                cheatBuffer.clear();
            }
        }
    }
    // ========================================

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P) {
            cheatBuffer.clear();
            SoundManager::getInstance().stopCoinSound();
            stateManager->pushState(
                std::make_unique<PausedState>(window, factory, camera, stateManager, this, mapFile));
            return;
        }

        if (event.key.code == sf::Keyboard::Escape) {
            cheatBuffer.clear();
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
} // namespace representation
