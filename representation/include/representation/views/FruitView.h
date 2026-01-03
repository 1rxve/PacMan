#ifndef PACMANGAME_FRUITVIEW_H
#define PACMANGAME_FRUITVIEW_H

#include "EntityView.h"
#include "logic/entities/FruitModel.h"
#include <memory>

namespace representation {
/**
 * SFML view for rendering fruit power-ups.
 *
 * Fruits trigger fear mode when collected by PacMan, making all ghosts
 * vulnerable and reversing their direction.
 *
 * Automatically hides when FruitModel::isCollected() returns true.
 * Respawns for next level like coins.
 *
 * Sprite: 50x50 pixel cherry (scaled to 0.7x) from shared sprite sheet.
 */
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