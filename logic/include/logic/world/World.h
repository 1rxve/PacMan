#ifndef PACMANGAME_WORLD_H
#define PACMANGAME_WORLD_H

#include "logic/entities/CoinModel.h"
#include "logic/entities/DoorModel.h"
#include "logic/entities/EntityModel.h"
#include "logic/entities/FruitModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/entities/NoEntryModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/patterns/AbstractFactory.h"
#include "logic/patterns/Observer.h"
#include "logic/utils/Score.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace logic {
class PacManModel;
class WallModel;
class CoinModel;

class World {
private:
    std::vector<std::shared_ptr<EntityModel>> entities;
    std::vector<std::unique_ptr<Observer>> wallViews;
    std::vector<std::unique_ptr<Observer>> coinViews;
    std::vector<std::unique_ptr<Observer>> fruitViews;
    std::vector<std::unique_ptr<Observer>> doorViews;
    std::vector<std::unique_ptr<Observer>> ghostViews;
    std::unique_ptr<Observer> pacmanView;

    AbstractFactory* factory;

    std::shared_ptr<PacManModel> pacman;
    std::vector<std::shared_ptr<WallModel>> walls;
    std::vector<std::shared_ptr<CoinModel>> coins;
    std::vector<std::shared_ptr<GhostModel>> ghosts;
    std::vector<std::shared_ptr<DoorModel>> doors;
    std::vector<std::shared_ptr<NoEntryModel>> noEntries;
    std::vector<std::shared_ptr<FruitModel>> fruits;

    int coinsCollected;
    Score score;
    Subject scoreSubject;

    float pacmanSpawnX;
    float pacmanSpawnY;
    std::vector<std::pair<float, float>> ghostSpawnPositions;

    bool hasJustRespawned;

    bool fearModeActive;
    float fearModeTimer;

    int currentLevel;
    float baseGhostSpeed;
    float baseFearDuration;

    void handlePacManDeath();
    void resetAfterDeath();

public:
    World();

    ~World();

    void update(float deltaTime);

    void setFactory(AbstractFactory* factory);

    void addEntity(std::unique_ptr<EntityModel> entity);

    void loadMap(const std::string& filename);

    std::shared_ptr<PacManModel> getPacMan();

    static std::pair<int, int> getMapDimensions(const std::string& filename);

    bool isDirectionValid(Direction direction) const;
    Direction getViableDirectionForGhost(std::shared_ptr<GhostModel> ghost) const;
    std::vector<Direction> getViableDirectionsForGhost(std::shared_ptr<GhostModel> ghost) const;

    void clearWorld();

    void notifyViewsOnly();

    void renderInOrder();

    void activateFearMode();

    bool justRespawned() const { return hasJustRespawned; }
    void clearRespawnFlag() { hasJustRespawned = false; }

    int getCoinsCollected() const { return coinsCollected; }
    int getTotalCoins() const { return static_cast<int>(coins.size()); }

    int getScore() const { return score.getScore(); }
    Score* getScoreObject() { return &score; }
    Subject* getScoreSubject() { return &scoreSubject; }

    int getCurrentLevel() const { return currentLevel; }
    void nextLevel();
};
} // namespace logic

#endif // PACMANGAME_WORLD_H