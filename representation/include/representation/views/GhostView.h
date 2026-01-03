#ifndef PACMANGAME_GHOSTVIEW_H
#define PACMANGAME_GHOSTVIEW_H

#include "EntityView.h"
#include "logic/entities/GhostModel.h"
#include <memory>

namespace representation {
/**
 * SFML view for rendering ghosts with state-dependent sprites.
 *
 * Sprite selection based on GhostState:
 * - CHASING/EXITING_SPAWN: Color-coded (red/pink/blue/orange) + direction + animation
 * - FEAR: Blue sprite with white flicker warning in last 1.5s
 * - EATEN: Eyes-only sprite navigating back to spawn
 * - RESPAWNING: Flicker between eyes and normal sprite (6 flickers × 0.3s)
 *
 * Animation: 2-frame walking cycle, switches every 0.15s when moving.
 * Direction sprites: separate sprites for UP/DOWN/LEFT/RIGHT movement.
 */
class GhostView : public EntityView {
private:
    logic::GhostModel* ghostModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    bool showDebugVisualization; // Green collision box + red center dot (development aid)

    float animationTimer;
    int frameIndex;

public:
    GhostView(logic::GhostModel* model, sf::RenderWindow* window, const Camera* camera,
              std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_GHOSTVIEW_H