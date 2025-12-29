#ifndef PACMANGAME_WORLD_H
#define PACMANGAME_WORLD_H

#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <iostream>
#include "logic/entities/EntityModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/entities/DoorModel.h"
#include "logic/entities/NoEntryModel.h"
#include "logic/patterns/AbstractFactory.h"
#include "logic/patterns/Observer.h"
#include "logic/utils/Score.h"
#include "logic/entities/FruitModel.h"

namespace logic {
    class PacManModel;
    class WallModel;
    class CoinModel;

    class World {
    private:
        std::vector<std::unique_ptr<EntityModel> > entities;
        std::vector<std::unique_ptr<Observer>> views;
        AbstractFactory* factory;

        // Non-owning references to entities for performance optimization.
        // Lifetime: Guaranteed by entities vector ownership (unique_ptr).
        // Usage: Cached pointers to avoid searching entities vector in game loop.
        PacManModel* pacman;
        std::vector<WallModel*> walls;
        std::vector<CoinModel*> coins;
        std::vector<GhostModel*> ghosts;
        std::vector<DoorModel*> doors;
        std::vector<NoEntryModel*> noEntries;
        std::vector<FruitModel*> fruits;


        int coinsCollected;
        Score score;
        Subject scoreSubject;

        float pacmanSpawnX;
        float pacmanSpawnY;
        std::vector<std::pair<float, float>> ghostSpawnPositions;

        bool hasJustRespawned;

        void handlePacManDeath();
        void resetAfterDeath();


    public:
        World();

        ~World();

        void update(float deltaTime);

        void setFactory(AbstractFactory* factory);

        void addEntity(std::unique_ptr<EntityModel> entity);

        void loadMap(const std::string& filename);

        PacManModel* getPacMan();

        static std::pair<int, int> getMapDimensions(const std::string& filename);

        bool isDirectionValid(Direction direction) const;
        Direction getViableDirectionForGhost(GhostModel* ghost) const;
        std::vector<Direction> getViableDirectionsForGhost(GhostModel* ghost) const;  // ← ADD

        void clearWorld();

        void notifyViewsOnly();

        bool justRespawned() const { return hasJustRespawned; }  // ← ADD
        void clearRespawnFlag() { hasJustRespawned = false; }


        int getCoinsCollected() const { return coinsCollected; }
        int getTotalCoins() const { return static_cast<int>(coins.size()); }

        int getScore() const { return score.getScore(); }
        Score* getScoreObject() { return &score; }
        Subject* getScoreSubject() { return &scoreSubject; }
    };
}

#endif //PACMANGAME_WORLD_H