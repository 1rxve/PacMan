#include "representation/views/CoinView.h"
#include <iostream>

namespace representation {
    CoinView::CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera,
                       std::shared_ptr<sf::Texture> sharedTexture)
            : EntityView(model, window, camera),
              coinModel(model),
              texture(sharedTexture) {  // ← Store shared_ptr

        // Gebruik shared texture (NIET opnieuw laden)
        sprite.setTexture(*texture);
        sprite.setTextureRect(sf::IntRect(410, 208, 6, 6));
        sprite.setOrigin(3.0f, 3.0f);
    }

    void CoinView::draw() {
        // Als coin collected is, render niet
        if (coinModel->isCollected()) {
            return;
        }

        // Get center position van coin
        float centerX = coinModel->getX();
        float centerY = coinModel->getY();

        // Convert naar pixel coordinates
        float pixelX = camera->normalizedToPixelX(centerX);
        float pixelY = camera->normalizedToPixelY(centerY);

        // Render coin sprite
        sprite.setPosition(pixelX, pixelY);
        sprite.setScale(1.0f, 1.0f);  // Adjust als nodig
        window->draw(sprite);
    }
}