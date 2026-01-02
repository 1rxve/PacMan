#ifndef PACMANGAME_PACMANVIEW_H
#define PACMANGAME_PACMANVIEW_H

#include "EntityView.h"
#include "logic/entities/PacManModel.h"
#include <memory> // ← Voor shared_ptr

namespace representation {
struct SpriteRect {
    int x;
    int y;
    int width;
    int height;
};

class PacManView : public EntityView {
private:
    logic::PacManModel* pacManModel;

    std::shared_ptr<sf::Texture> texture; // ← SHARED_PTR (niet sf::Texture texture)
    sf::Sprite sprite;

    float animationTimer;
    int frameIndex;
    const int frames[4] = {0, 1, 2, 1};

    sf::Clock animationClock;

    bool showDebugVisualization = false;

    void renderDeathAnimation();

public:
    PacManView(logic::PacManModel* model, sf::RenderWindow* window, const Camera* camera,
               std::shared_ptr<sf::Texture> sharedTexture); // ← SHARED_PTR parameter

    void draw() override;
};
} // namespace representation

#endif