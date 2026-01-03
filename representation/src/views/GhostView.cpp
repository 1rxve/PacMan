#include "representation/views/GhostView.h"

namespace representation {
GhostView::GhostView(logic::GhostModel* model, sf::RenderWindow* window, const Camera* camera,
                     std::shared_ptr<sf::Texture> sharedTexture)
    : EntityView(model, window, camera), ghostModel(model), texture(sharedTexture), showDebugVisualization(false),
      animationTimer(0.0f), frameIndex(0) {

    sprite.setTexture(*texture);
    sprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
}

void GhostView::draw() {
    logic::GhostType type = ghostModel->getType();
    logic::Direction direction = ghostModel->getCurrentDirection();
    logic::GhostState state = ghostModel->getState();

    // 2-frame animation: switch every 0.15s when moving
    if (direction != logic::Direction::NONE) {
        animationTimer += 0.016f; // Approximate frame time at 60 FPS

        if (animationTimer >= 0.15f) {
            frameIndex = (frameIndex == 0) ? 1 : 0;
            animationTimer = 0.0f;
        }
    }

    int spriteX = 0;
    int spriteY = 0;

    // State-driven sprite selection: FEAR mode with flicker warning
    if (state == logic::GhostState::FEAR) {
        float fearTimer = ghostModel->getFearTimer();
        const float FLICKER_THRESHOLD = 1.5f; // Start flicker warning at 1.5s remaining

        if (fearTimer < FLICKER_THRESHOLD && fearTimer > 0.0f) {
            bool showWhite = (static_cast<int>(fearTimer / 0.2f) % 2 == 0);
            spriteX = showWhite ? 50 : 0;
        } else {
            spriteX = 0;
        }

        spriteY = 550 + (frameIndex * 50);
    }
    // EATEN mode: eyes-only sprite navigating back to spawn
    else if (state == logic::GhostState::EATEN) {
        spriteX = 300;
        spriteY = 250;
    }
    // RESPAWNING mode: flicker between eyes and normal sprite (6 flickers total)
    else if (state == logic::GhostState::RESPAWNING) {
        int flickerCount = ghostModel->getRespawnFlickerCount();

        if (flickerCount % 2 == 0) {
            spriteX = 300;
            spriteY = 250;
        } else {
            switch (type) {
            case logic::GhostType::RED:
                spriteX = 0;
                break;
            case logic::GhostType::PINK:
                spriteX = 50;
                break;
            case logic::GhostType::BLUE:
                spriteX = 100;
                break;
            case logic::GhostType::ORANGE:
                spriteX = 150;
                break;
            }
            spriteY = 0;
        }
    }
    // NORMAL/CHASING/EXITING_SPAWN modes: color-coded + directional + animated
    else {
        switch (type) {
        case logic::GhostType::RED:
            spriteX = 0;
            break;
        case logic::GhostType::PINK:
            spriteX = 50;
            break;
        case logic::GhostType::BLUE:
            spriteX = 100;
            break;
        case logic::GhostType::ORANGE:
            spriteX = 150;
            break;
        }

        switch (direction) {
        case logic::Direction::RIGHT:
            spriteY = (frameIndex == 0) ? 0 : 50;
            break;
        case logic::Direction::DOWN:
            spriteY = (frameIndex == 0) ? 100 : 150;
            break;
        case logic::Direction::LEFT:
            spriteY = (frameIndex == 0) ? 200 : 250;
            break;
        case logic::Direction::UP:
            spriteY = (frameIndex == 0) ? 300 : 350;
            break;
        case logic::Direction::NONE:
            spriteY = 0;
            break;
        }
    }

    sprite.setTextureRect(sf::IntRect(spriteX, spriteY, 50, 50));

    float centerX = ghostModel->getX();
    float centerY = ghostModel->getY();

    float pixelCenterX = camera->normalizedToPixelX(centerX);
    float pixelCenterY = camera->normalizedToPixelY(centerY);

    sprite.setOrigin(19.0f, 22.0f); // Empirically tuned for accurate center-point rendering
    sprite.setPosition(pixelCenterX, pixelCenterY);
    sprite.setScale(1.15f, 1.15f);
    window->draw(sprite);

    // Debug visualization: green sprite box + red center dot
    if (showDebugVisualization) {
        sf::RectangleShape debugBox(sf::Vector2f(50.0f, 50.0f));
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Green);
        debugBox.setOutlineThickness(1.0f);
        debugBox.setOrigin(25.0f, 25.0f);
        debugBox.setPosition(pixelCenterX, pixelCenterY);
        window->draw(debugBox);

        sf::CircleShape debugCircle(3.0f);
        debugCircle.setFillColor(sf::Color::Red);
        debugCircle.setOrigin(3.0f, 3.0f);
        debugCircle.setPosition(pixelCenterX, pixelCenterY);
        window->draw(debugCircle);
    }
}
} // namespace representation