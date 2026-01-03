#include "representation/views/DoorView.h"

namespace representation {
DoorView::DoorView(logic::DoorModel* model, sf::RenderWindow* window, const Camera* camera,
                   std::shared_ptr<sf::Texture> sharedTexture)
    : EntityView(model, window, camera), doorModel(model), texture(sharedTexture) {

    sprite.setTexture(*texture);
    sprite.setTextureRect(sf::IntRect(636, 520, 15, 5)); // Horizontal door bar from sprite sheet
    sprite.setOrigin(7.0f, 7.0f);
}

void DoorView::draw() {
    float centerX = doorModel->getX();
    float centerY = doorModel->getY();

    float pixelCenterX = camera->normalizedToPixelX(centerX);
    float pixelCenterY = camera->normalizedToPixelY(centerY);

    sprite.setPosition(pixelCenterX, pixelCenterY - 3.0f); // -3.0f vertical adjustment for visual alignment
    sprite.setScale(4.5f, 3.5f);                           // Scale 15x5 sprite to visible door size
    window->draw(sprite);

    // Debug visualization: green outline showing collision bounds
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

        sf::RectangleShape debugBox(sf::Vector2f(pixelWidth, pixelHeight));
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Green);
        debugBox.setOutlineThickness(2.0f);
        debugBox.setPosition(pixelTopLeftX, pixelTopLeftY);
        window->draw(debugBox);
    }
}
} // namespace representation