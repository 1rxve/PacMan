#include "representation/views/CoinView.h"
#include <iostream>

namespace representation {
    CoinView::CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera)
            : EntityView(model, window, camera),
              coinModel(model) {

        // Load sprite sheet
        if (!texture.loadFromFile("resources/sprites/pacman_sprites.png")) {
            std::cerr << "ERROR: Could not load pacman sprites!" << std::endl;
        }

        sprite.setTexture(texture);

        // Coin sprite coordinates - KLEINE WITTE DOT
        // Check je sprite sheet voor exacte coordinaten
        // Meestal zijn coins kleine dots in de sheet
        sprite.setTextureRect(sf::IntRect(410, 208, 6, 6));

        // Origin op center voor correcte positioning
        sprite.setOrigin(5.0f, 5.0f);  // Half van 10x10
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