#include "representation/views/FruitView.h"

namespace representation {
FruitView::FruitView(logic::FruitModel* model, sf::RenderWindow* window, const Camera* camera,
                     std::shared_ptr<sf::Texture> sharedTexture)
    : EntityView(model, window, camera), fruitModel(model), texture(sharedTexture) {

    sprite.setTexture(*texture);
    // Cherry sprite (example coords - we'll verify later)
    sprite.setTextureRect(sf::IntRect(550, 300, 50, 50));
    sprite.setOrigin(18.5f, 24.5f);
}

void FruitView::draw() {
    // Don't render if collected
    if (fruitModel->isCollected()) {
        return;
    }

    float centerX = fruitModel->getX();
    float centerY = fruitModel->getY();

    float pixelCenterX = camera->normalizedToPixelX(centerX);
    float pixelCenterY = camera->normalizedToPixelY(centerY);

    sprite.setPosition(pixelCenterX, pixelCenterY);
    sprite.setScale(0.7f, 0.7f); // Scale up fruit
    window->draw(sprite);
}
} // namespace representation