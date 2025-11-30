#include "representation/ConcreteFactory.h"
#include "representation/views/WallView.h"
#include "representation/views/PacManView.h"
#include "logic/entities/WallModel.h"
#include "logic/entities/PacManModel.h"

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
}