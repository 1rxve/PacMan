#include "representation/ConcreteFactory.h"
#include "representation/views/WallView.h"
#include "representation/views/PacManView.h"
#include "representation/views/CoinView.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"
#include "logic/entities/CoinModel.h"

namespace representation {
    ConcreteFactory::ConcreteFactory(sf::RenderWindow* window, const Camera* camera)
            : window(window), camera(camera) {
    }

    logic::EntityCreationResult ConcreteFactory::createWall(float x, float y, float w, float h) {
        // Maak Model
        auto model = std::make_unique<logic::WallModel>(x, y, w, h);

        // Maak View (moet raw pointer naar model hebben)
        auto view = std::make_unique<WallView>(model.get(), window, camera);

        // Return beide als unique_ptr
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createPacMan(float x, float y, float w, float h, float speed) {
        // Maak Model
        auto model = std::make_unique<logic::PacManModel>(x, y, w, h, speed);

        // Maak View
        auto view = std::make_unique<PacManView>(model.get(), window, camera);

        // Return beide als unique_ptr
        return {std::move(model), std::move(view)};
    }

    logic::EntityCreationResult ConcreteFactory::createCoin(float x, float y, float w, float h) {
        auto model = std::make_unique<logic::CoinModel>(x, y, w, h);
        auto view = std::make_unique<CoinView>(model.get(), window, camera);
        return {std::move(model), std::move(view)};
    }
}