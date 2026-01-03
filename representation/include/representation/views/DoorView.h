#ifndef PACMANGAME_DOORVIEW_H
#define PACMANGAME_DOORVIEW_H

#include "EntityView.h"
#include "logic/entities/DoorModel.h"
#include <memory>

namespace representation {
/**
 * SFML view for rendering ghost house door.
 *
 * Door blocks PacMan from entering spawn area but allows ghosts to pass through
 * when exiting spawn or returning after being eaten (collision logic in World).
 *
 * Sprite: 15x5 pixel region scaled to 4.5x3.5 for visual clarity.
 */
class DoorView : public EntityView {
private:
    logic::DoorModel* doorModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    bool showDebugVisualization = false; // Green collision box overlay (development aid)

public:
    DoorView(logic::DoorModel* model, sf::RenderWindow* window, const Camera* camera,
             std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_DOORVIEW_H