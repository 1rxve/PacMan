#include "representation/states/LevelState.h"
#include "representation/states/PausedState.h"  // ← ADD
#include "representation/StateManager.h"          // ← ADD
#include "logic/entities/PacManModel.h"
#include <iostream>

namespace representation {
    LevelState::~LevelState() {
        // Detach Score observer
        if (world) {
            world->getScoreSubject()->detach(world->getScoreObject());
        }
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

            // Score text
            scoreText.setFont(font);
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::Yellow);
            scoreText.setPosition(10.0f, 10.0f);

            // Lives text
            livesText.setFont(font);
            livesText.setCharacterSize(24);
            livesText.setFillColor(sf::Color::White);
            livesText.setPosition(10.0f, 40.0f);

            std::cout << "LevelState: Font loaded for UI" << std::endl;
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

            std::cerr << "LevelState: Using fallback font" << std::endl;
        } else {
            std::cerr << "LevelState: Could not load font, UI disabled" << std::endl;
        }

        // Setup PacMan cell dimensions
        logic::PacManModel* pacman = world->getPacMan();
        if (!pacman) {
            std::cerr << "ERROR: PacMan not found in map!" << std::endl;
            return;
        }


        // Calculate cell dimensions
        auto [mapWidth, mapHeight] = logic::World::getMapDimensions(mapFile);
        float cellWidth = 2.0f / mapWidth;
        float cellHeight = 2.0f / mapHeight;
        pacman->setCellDimensions(cellWidth, cellHeight);

        world->getScoreSubject()->attach(world->getScoreObject());

    }

    void LevelState::update(float deltaTime) {
        world->update(deltaTime);

        // Update UI text
        if (fontLoaded) {
            scoreText.setString("SCORE: " + std::to_string(world->getScore()));

            logic::PacManModel* pacman = world->getPacMan();
            if (pacman) {
                livesText.setString("LIVES: " + std::to_string(pacman->getLives()));
            }
        }
    }

    void LevelState::render() {
        // Draw UI overlay
        if (fontLoaded) {
            window->draw(scoreText);
            window->draw(livesText);
        }
    }

    void LevelState::handleEvent(const sf::Event& event) {
        logic::PacManModel* pacman = world->getPacMan();
        if (!pacman) return;

        if (event.type == sf::Event::KeyPressed) {
            // P = pause
            if (event.key.code == sf::Keyboard::P) {
                stateManager->pushState(std::make_unique<PausedState>(
                        window, factory, camera, stateManager
                ));
                return;
            }

            // ESC = close game
            if (event.key.code == sf::Keyboard::Escape) {
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