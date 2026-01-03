#include "representation/views/CoinView.h"
#include <iostream>

namespace representation {
CoinView::CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera,
                   std::shared_ptr<sf::Texture> sharedTexture)
    : EntityView(model, window, camera), coinModel(model), texture(sharedTexture) {

    sprite.setTexture(*texture);
    sprite.setTextureRect(sf::IntRect(410, 208, 6, 6)); // Small coin dot from sprite sheet
    sprite.setOrigin(3.0f, 3.0f);                        // Center origin (half of 6x6)
}

void CoinView::draw() {
    if (coinModel->isCollected()) {
        return;
    }

    float centerX = coinModel->getX();
    float centerY = coinModel->getY();

    float pixelX = camera->normalizedToPixelX(centerX);
    float pixelY = camera->normalizedToPixelY(centerY);

    sprite.setPosition(pixelX, pixelY);
    sprite.setScale(1.0f, 1.0f);
    window->draw(sprite);
}
} // namespace representation