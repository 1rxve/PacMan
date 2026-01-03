#include "representation/states/LevelState.h"
#include "logic/entities/PacManModel.h"
#include "representation/Camera.h"
#include "representation/SoundManager.h"
#include "representation/StateManager.h"
#include "representation/states/NameEntryState.h"
#include "representation/states/PausedState.h"
#include "representation/states/VictoryState.h"

namespace representation {
// World owned by unique_ptr, auto-destructs and cascades cleanup to entities
LevelState::~LevelState() {}

LevelState::LevelState(sf::RenderWindow* win, logic::AbstractFactory* fac, const Camera* cam, StateManager* sm,
                       const std::string& mapFile)
    : State(win, fac, cam, sm), mapFile(mapFile), cheatBuffer("") {

    world = std::make_unique<logic::World>();
    world->setFactory(factory);

    try {
        world->loadMap(mapFile);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
    }

    // Load font with fallback (custom font → system font)
    fontLoaded = false;
    if (font.loadFromFile("resources/fonts/joystix.otf") || font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Yellow);
        scoreText.setPosition(10.0f, 10.0f);

        livesTexture = std::make_shared<sf::Texture>();
        if (livesTexture->loadFromFile("resources/sprites/pacman_sprites.png")) {
            livesSprite.setTexture(*livesTexture);
            livesSprite.setTextureRect(sf::IntRect(840, 0, 50, 50));
            livesSprite.setOrigin(25.0f, 25.0f);
            livesSprite.setScale(0.6f, 0.6f);
        }

        readyText.setFont(font);
        readyText.setString("READY!");
        readyText.setCharacterSize(35);
        readyText.setFillColor(sf::Color::Yellow);

        sf::FloatRect bounds = readyText.getLocalBounds();
        readyText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        readyText.setPosition(window->getSize().x / 2.0f, window->getSize().y / 2.0f - 7);
    }

    // Convert normalized coordinates [-1,1] to grid cell dimensions
    auto pacman = world->getPacMan();
    if (pacman) {
        auto [mapWidth, mapHeight] = logic::World::getMapDimensions(mapFile);
        float cellWidth = 2.0f / mapWidth;   // 2.0f = full normalized width
        float cellHeight = 2.0f / mapHeight; // 2.0f = full normalized height
        pacman->setCellDimensions(cellWidth, cellHeight);
    }

    SoundManager::getInstance().stopMenuMusic();

    soundObserver = std::make_unique<SoundObserver>(world->getScoreObject());
    world->getScoreSubject()->attach(world->getScoreObject());
    world->getScoreSubject()->attach(soundObserver.get());

    // Initial countdown before gameplay starts
    isCountingDown = true;
    countdownTimer = 1.0f; // 1 second "READY!" display
}

void LevelState::update(float deltaTime) {
    if (isCountingDown) {
        countdownTimer -= deltaTime;

        if (countdownTimer <= 0.0f) {
            isCountingDown = false;
            world->clearRespawnFlag();
        }
        return;
    }

    SoundManager::getInstance().update(deltaTime);

    // Respawn triggers new countdown (after death)
    if (world->justRespawned()) {
        isCountingDown = true;
        countdownTimer = 1.0f; // 1 second pause after respawn
        SoundManager::getInstance().stopCoinSound();
        return;
    }

    world->update(deltaTime);

    if (fontLoaded) {
        scoreText.setString("SCORE: " + std::to_string(world->getScore()));
    }

    int coinsCollected = world->getCoinsCollected();
    int totalCoins = world->getTotalCoins();

    // Victory: All coins collected → next level
    if (coinsCollected >= totalCoins) {
        world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
        world->getScoreSubject()->notify();

        SoundManager::getInstance().stopCoinSound();

        world->nextLevel();

        isCountingDown = true;
        countdownTimer = 2.0f; // 2 second "READY!" between levels
        return;
    }

    // Game Over: Lives depleted → high score entry or victory screen
    auto pacman = world->getPacMan();
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

    if (isCountingDown && fontLoaded) {
        window->draw(readyText);
    }

    if (fontLoaded) {
        window->draw(scoreText);

        auto pacman = world->getPacMan();
        if (pacman && livesTexture) {
            int lives = pacman->getLives();
            for (int i = 0; i < lives; i++) {
                livesSprite.setPosition(20.0f + i * 35.0f, 55.0f);
                window->draw(livesSprite);
            }
        }

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
    auto pacman = world->getPacMan();
    if (!pacman)
        return;

    // Cheat code system: buffer last 10 chars typed, search for keywords
    if (event.type == sf::Event::TextEntered) {
        char typed = static_cast<char>(event.text.unicode);

        if ((typed >= 'a' && typed <= 'z') || (typed >= 'A' && typed <= 'Z')) {
            if (typed >= 'a' && typed <= 'z') {
                typed = typed - 'a' + 'A';
            }

            cheatBuffer += typed;

            // Sliding window: keep only last 10 chars to prevent memory growth
            if (cheatBuffer.length() > 10) {
                cheatBuffer = cheatBuffer.substr(1);
            }

            // "GHOST" cheat: activate fear mode
            if (cheatBuffer.find("GHOST") != std::string::npos) {
                world->activateFearMode();
                cheatBuffer.clear();
            }

            // "LEVEL" cheat: skip to next level
            if (cheatBuffer.find("LEVEL") != std::string::npos) {
                world->getScoreObject()->setEvent(logic::ScoreEvent::LEVEL_CLEARED);
                world->getScoreSubject()->notify();

                world->nextLevel();
                isCountingDown = true;
                countdownTimer = 2.0f;
                cheatBuffer.clear();
            }
        }
    }

    if (event.type == sf::Event::KeyPressed) {
        // Pause game
        if (event.key.code == sf::Keyboard::P) {
            cheatBuffer.clear();
            SoundManager::getInstance().stopCoinSound();
            stateManager->pushState(
                std::make_unique<PausedState>(window, factory, camera, stateManager, this, mapFile));
            return;
        }

        // Quit game
        if (event.key.code == sf::Keyboard::Escape) {
            cheatBuffer.clear();
            window->close();
            return;
        }

        // Arrow key input → buffered direction (applied when valid in World::update)
        if (event.key.code == sf::Keyboard::Up && pacman->getCurrentDirection() != logic::Direction::UP) {
            pacman->setNextDirection(logic::Direction::UP);
        } else if (event.key.code == sf::Keyboard::Down && pacman->getCurrentDirection() != logic::Direction::DOWN) {
            pacman->setNextDirection(logic::Direction::DOWN);
        } else if (event.key.code == sf::Keyboard::Left && pacman->getCurrentDirection() != logic::Direction::LEFT) {
            pacman->setNextDirection(logic::Direction::LEFT);
        } else if (event.key.code == sf::Keyboard::Right && pacman->getCurrentDirection() != logic::Direction::RIGHT) {
            pacman->setNextDirection(logic::Direction::RIGHT);
        }
    }
}
} // namespace representation