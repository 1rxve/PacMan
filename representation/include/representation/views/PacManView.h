#ifndef PACMANGAME_PACMANVIEW_H
#define PACMANGAME_PACMANVIEW_H

#include "EntityView.h"
#include "logic/entities/PacManModel.h"
#include <memory>

namespace representation {
/**
 * Helper struct for sprite region selection from sprite sheet.
 * Avoids verbose sf::IntRect constructor calls in animation logic.
 */
struct SpriteRect {
    int x;
    int y;
    int width;
    int height;
};

/**
 * SFML view for rendering PacMan with direction-based animation.
 *
 * Animation states:
 * - Stationary: Full circle sprite (mouth closed)
 * - Moving: 4-frame mouth cycle {closed, half-open, full-open, half-open}
 *   switches every 0.1s, creating smooth chomping effect
 * - Dying: 11-frame death animation over 2 seconds (handled by renderDeathAnimation())
 *
 * Direction sprites: Separate sprite sets for UP/DOWN/LEFT/RIGHT movement.
 * Origin manually tuned to (29.5, 22.0) for accurate center-point rendering.
 */
class PacManView : public EntityView {
private:
    logic::PacManModel* pacManModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    float animationTimer;
    int frameIndex;
    const int frames[4] = {0, 1, 2, 1}; // Mouth cycle: closed → half → full → half → repeat

    sf::Clock animationClock;

    bool showDebugVisualization = false; // Green sprite box + red center dot (development aid)

    /**
     * Renders 11-frame death animation based on PacManModel::getDeathTimer().
     * Each frame is 50x50 pixels from sprite sheet, displayed over 2 seconds.
     * Final frame (index 10) uses different origin (20.0, 27.0) for visual alignment.
     */
    void renderDeathAnimation();

public:
    PacManView(logic::PacManModel* model, sf::RenderWindow* window, const Camera* camera,
               std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif