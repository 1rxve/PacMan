#include "representation/views/GhostView.h"


namespace representation {
    GhostView::GhostView(logic::GhostModel* model, sf::RenderWindow* window,
                         const Camera* camera, std::shared_ptr<sf::Texture> sharedTexture)
            : EntityView(model, window, camera),
              ghostModel(model),
              texture(sharedTexture),
              showDebugVisualization(true) {  // ← ADD (zet op false voor release)

        sprite.setTexture(*texture);
        sprite.setTextureRect(sf::IntRect(0, 0, 50, 50));
    }

    void GhostView::draw() {
        logic::GhostType type = ghostModel->getType();

        // Sprite selection code...
        int spriteX = 0;
        int spriteY = 0;  // Second row 45

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

        sprite.setTextureRect(sf::IntRect(spriteX, spriteY, 50, 50));

        // Render position
        float centerX = ghostModel->getX();
        float centerY = ghostModel->getY();

        float pixelCenterX = camera->normalizedToPixelX(centerX);
        float pixelCenterY = camera->normalizedToPixelY(centerY);

        // Sprite rendering
        sprite.setOrigin(19.0f, 22.0f);  // Adjust as needed
        sprite.setPosition(pixelCenterX, pixelCenterY);
        sprite.setScale(1.0f, 1.0f);
        window->draw(sprite);

        // DEBUG VISUALISATIE (EXACT ZOALS PACMAN) ←←←←
        if (showDebugVisualization) {
            // Groen vierkant = sprite texture bounds
            sf::RectangleShape debugBox(sf::Vector2f(50.0f, 50.0f));
            debugBox.setFillColor(sf::Color::Transparent);
            debugBox.setOutlineColor(sf::Color::Green);
            debugBox.setOutlineThickness(1.0f);
            debugBox.setOrigin(25.0f, 25.0f);  // Zelfde origin als sprite
            debugBox.setPosition(pixelCenterX, pixelCenterY);
            window->draw(debugBox);

            // Rode dot = logische center positie
            sf::CircleShape debugCircle(3.0f);
            debugCircle.setFillColor(sf::Color::Red);
            debugCircle.setOrigin(3.0f, 3.0f);
            debugCircle.setPosition(pixelCenterX, pixelCenterY);
            window->draw(debugCircle);
        }
    }
}