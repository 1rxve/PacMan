#ifndef PACMANGAME_FRUITVIEW_H
#define PACMANGAME_FRUITVIEW_H

#include "EntityView.h"
#include "logic/entities/FruitModel.h"
#include <memory>

namespace representation {
class FruitView : public EntityView {
private:
    logic::FruitModel* fruitModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

public:
    FruitView(logic::FruitModel* model, sf::RenderWindow* window, const Camera* camera,
              std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_FRUITVIEW_H