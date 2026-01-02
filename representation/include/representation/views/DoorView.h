#ifndef PACMANGAME_DOORVIEW_H
#define PACMANGAME_DOORVIEW_H

#include "EntityView.h"
#include "logic/entities/DoorModel.h"
#include <memory>

namespace representation {
class DoorView : public EntityView {
private:
    logic::DoorModel* doorModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    bool showDebugVisualization = false;

public:
    DoorView(logic::DoorModel* model, sf::RenderWindow* window, const Camera* camera,
             std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_DOORVIEW_H