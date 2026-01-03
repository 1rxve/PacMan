#ifndef PACMANGAME_WALLVIEW_H
#define PACMANGAME_WALLVIEW_H

#include "EntityView.h"
#include "logic/entities/WallModel.h"

namespace representation {
/**
 * SFML view for rendering maze walls as blue rectangles.
 *
 * Uses sf::RectangleShape instead of sprites for simplicity.
 * Size converted from normalized WallModel dimensions to pixel coordinates via Camera.
 */
class WallView : public EntityView {
private:
    sf::RectangleShape shape;

public:
    WallView(logic::WallModel* model, sf::RenderWindow* window, const Camera* camera);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_WALLVIEW_H