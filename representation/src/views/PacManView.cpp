#include "representation/views/PacManView.h"
#include <iostream>

namespace representation {
    PacManView::PacManView(logic::PacManModel *model)
        : model(model),
        animationTimer(0.0f),
        frameIndex(0) {
        if (!texture.loadFromFile("resources/sprites/pacman_sprites.png")) {
            std::cerr << "ERROR: Could not load pacman sprites!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(853, 5, 33, 33));
    }

    void PacManView::update(float deltaTime) {
        animationTimer += deltaTime;

        if (animationTimer >= 0.1f) {
            frameIndex = (frameIndex + 1) % 4;
            animationTimer = 0.0f;

            int currentFrame = frames[frameIndex];
            SpriteRect rect = {853, 5, 33, 33}; // Default waarde


            switch (model->getCurrentDirection()) {
                case logic::Direction::RIGHT:
                    if (currentFrame == 0) {
                        rect = {840, 0, 50, 50};
                    } else if (currentFrame == 1) {
                        rect = {840, 50, 50, 50};
                    } else if (currentFrame == 2) {
                        rect = {840, 100, 50, 50};
                    }
                    break;

                case logic::Direction::LEFT:
                    if (currentFrame == 0) {
                        rect = {840, 300, 50, 50};
                    } else if (currentFrame == 1) {
                        rect = {840, 350, 50, 50};
                    } else if (currentFrame == 2) {
                        rect = {840, 400, 50, 50};
                    }
                    break;

                case logic::Direction::UP:
                    if (currentFrame == 0) {
                        rect = {840, 450, 50, 50};
                    } else if (currentFrame == 1) {
                        rect = {840, 500, 50, 50};
                    } else if (currentFrame == 2) {
                        rect = {840, 550, 50, 50};
                    }
                    break;

                case logic::Direction::DOWN:
                    if (currentFrame == 0) {
                        rect = {840, 150, 50, 50};
                    } else if (currentFrame == 1) {
                        rect = {840, 200, 50, 50};
                    } else if (currentFrame == 2) {
                        rect = {840, 250, 50, 50};
                    }
                    break;

                case logic::Direction::NONE:
                    rect = {840, 0, 50, 50};
                    break;
            }

            sprite.setTextureRect(sf::IntRect(rect.x, rect.y, rect.width, rect.height));
        }
    }

    void PacManView::draw(sf::RenderWindow &window, const Camera &camera) {
        float pixelX = camera.normalizedToPixelX(model->getX());
        float pixelY = camera.normalizedToPixelY(model->getY());

        sprite.setPosition(pixelX, pixelY);
        sprite.setScale(2.0f, 2.0f);

        window.draw(sprite);
    }
}
