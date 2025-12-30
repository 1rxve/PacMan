#include "representation/views/GhostView.h"


namespace representation {
    GhostView::GhostView(logic::GhostModel* model, sf::RenderWindow* window,
                         const Camera* camera, std::shared_ptr<sf::Texture> sharedTexture)
            : EntityView(model, window, camera),
              ghostModel(model),
              texture(sharedTexture),
              showDebugVisualization(false),
              animationTimer(0.0f),
              frameIndex(0) {

        sprite.setTexture(*texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
    }

    void GhostView::draw() {
        logic::GhostType type = ghostModel->getType();
        logic::Direction direction = ghostModel->getCurrentDirection();
        logic::GhostState state = ghostModel->getState();

        // Update animation (only when moving)
        if (direction != logic::Direction::NONE) {
            animationTimer += 0.016f;  // ~60 FPS frame time

            if (animationTimer >= 0.15f) {  // Switch frame every 0.15s
                frameIndex = (frameIndex == 0) ? 1 : 0;
                animationTimer = 0.0f;
            }
        }

        int spriteX = 0;
        int spriteY = 0;

        // FEAR MODE - Blue sprite (overrides everything)
        if (state == logic::GhostState::FEAR) {
            spriteX = 0;
            spriteY = 550 + (frameIndex * 50);  // Y=550 or Y=600
        }
            // EATEN MODE - Eyes sprite (only during eaten, not during exit)
        else if (state == logic::GhostState::EATEN) {
            spriteX = 300;
            spriteY = 250;
        }
            // NORMAL MODE - Color + direction + animation
        else {
            // Determine base X position based on ghost type
            switch (type) {
                case logic::GhostType::RED:    spriteX = 0;   break;
                case logic::GhostType::PINK:   spriteX = 50;  break;
                case logic::GhostType::BLUE:   spriteX = 100; break;
                case logic::GhostType::ORANGE: spriteX = 150; break;
            }

            // Determine Y position based on direction + frame
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
                    spriteY = 0;  // Default to RIGHT frame 1
                    break;
            }
        }

        sprite.setTextureRect(sf::IntRect(spriteX, spriteY, 50, 50));

        // Render position
        float centerX = ghostModel->getX();
        float centerY = ghostModel->getY();

        float pixelCenterX = camera->normalizedToPixelX(centerX);
        float pixelCenterY = camera->normalizedToPixelY(centerY);

        // Sprite rendering
        sprite.setOrigin(19.0f, 22.0f);
        sprite.setPosition(pixelCenterX, pixelCenterY);
        sprite.setScale(1.15f, 1.15f);
        window->draw(sprite);

        // DEBUG VISUALISATIE
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
}