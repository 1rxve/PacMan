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
#include "logic/patterns/AbstractFactory.h"
#include "logic/patterns/Observer.h"

namespace logic {
    class PacManModel;
    class WallModel;
    class CoinModel;

    class World {
    private:
        std::vector<std::unique_ptr<EntityModel> > entities;
        std::vector<std::unique_ptr<Observer>> views;
        AbstractFactory* factory;
        PacManModel* pacman;
        std::vector<WallModel*> walls;
        std::vector<CoinModel*> coins;

        int coinsCollected;


    public:
        World() : factory(nullptr), pacman(nullptr) {}

        void update(float deltaTime);

        void setFactory(AbstractFactory* factory);

        void addEntity(std::unique_ptr<EntityModel> entity);

        void loadMap(const std::string& filename);

        PacManModel* getPacMan();

        static std::pair<int, int> getMapDimensions(const std::string& filename);

        bool isDirectionValid(Direction direction) const;

        int getCoinsCollected() const { return coinsCollected; }
        int getTotalCoins() const { return static_cast<int>(coins.size()); }
    };
}

#endif //PACMANGAME_WORLD_H