#ifndef PACMANGAME_WALLVIEW_H
#define PACMANGAME_WALLVIEW_H

#include "EntityView.h"
#include "logic/entities/WallModel.h"

namespace representation {
class WallView : public EntityView {
private:
    sf::RectangleShape shape;

public:
    WallView(logic::WallModel* model, sf::RenderWindow* window, const Camera* camera);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_WALLVIEW_H
