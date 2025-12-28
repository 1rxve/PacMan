#include "representation/views/DoorView.h"

namespace representation {
    DoorView::DoorView(logic::DoorModel* model, sf::RenderWindow* window,
                       const Camera* camera, std::shared_ptr<sf::Texture> sharedTexture)
            : EntityView(model, window, camera),
              doorModel(model),
              texture(sharedTexture) {

        sprite.setTexture(*texture);
        sprite.setTextureRect(sf::IntRect(636, 520, 15, 5));
        sprite.setOrigin(7.0f, 7.0f);
    }

    void DoorView::draw() {
        float centerX = doorModel->getX();
        float centerY = doorModel->getY();

        float pixelCenterX = camera->normalizedToPixelX(centerX);
        float pixelCenterY = camera->normalizedToPixelY(centerY);

        sprite.setPosition(pixelCenterX, pixelCenterY);
        sprite.setScale(3.5f, 3.5f);
        window->draw(sprite);

        // DEBUG VISUALISATIE
        if (showDebugVisualization) {
            float width = doorModel->getWidth();
            float height = doorModel->getHeight();

            float topLeftX = centerX - width / 2.0f;
            float topLeftY = centerY - height / 2.0f;
            float bottomRightX = centerX + width / 2.0f;
            float bottomRightY = centerY + height / 2.0f;

            float pixelTopLeftX = camera->normalizedToPixelX(topLeftX);
            float pixelTopLeftY = camera->normalizedToPixelY(topLeftY);
            float pixelBottomRightX = camera->normalizedToPixelX(bottomRightX);
            float pixelBottomRightY = camera->normalizedToPixelY(bottomRightY);

            float pixelWidth = pixelBottomRightX - pixelTopLeftX;
            float pixelHeight = pixelBottomRightY - pixelTopLeftY;

            // Groen vierkant = collision bounds
            sf::RectangleShape debugBox(sf::Vector2f(pixelWidth, pixelHeight));
            debugBox.setFillColor(sf::Color::Transparent);
            debugBox.setOutlineColor(sf::Color::Green);
            debugBox.setOutlineThickness(2.0f);
            debugBox.setPosition(pixelTopLeftX, pixelTopLeftY);
            window->draw(debugBox);
        }
    }
}