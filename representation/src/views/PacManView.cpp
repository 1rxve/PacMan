#include "representation/views/PacManView.h"
#include <iostream>

namespace representation {
    PacManView::PacManView(logic::PacManModel* model, sf::RenderWindow* window, const Camera* camera)
            : EntityView(model, window, camera),  // <- Base class constructor
              pacManModel(model),
              animationTimer(0.0f),
              frameIndex(0) {
        if (!texture.loadFromFile("resources/sprites/pacman_sprites.png")) {
            std::cerr << "ERROR: Could not load pacman sprites!" << std::endl;
        }
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::IntRect(840, 0, 50, 50));
    }



    void PacManView::draw() {
        // Alleen animeren als PacMan beweegt
        logic::Direction currentDir = pacManModel->getCurrentDirection();

        if (currentDir != logic::Direction::NONE) {
            // Update animatie
            float deltaTime = animationClock.restart().asSeconds();
            animationTimer += deltaTime;

            if (animationTimer >= 0.1f) {
                frameIndex = (frameIndex + 1) % 4;
                animationTimer = 0.0f;

                int currentFrame = frames[frameIndex];
                SpriteRect rect = {840, 0, 50, 50};

                switch (currentDir) {
                    case logic::Direction::RIGHT:
                        if (currentFrame == 0) rect = {840, 0, 50, 50};
                        else if (currentFrame == 1) rect = {840, 50, 50, 50};
                        else if (currentFrame == 2) rect = {840, 100, 50, 50};
                        break;

                    case logic::Direction::LEFT:
                        if (currentFrame == 0) rect = {840, 300, 50, 50};
                        else if (currentFrame == 1) rect = {840, 350, 50, 50};
                        else if (currentFrame == 2) rect = {840, 400, 50, 50};
                        break;

                    case logic::Direction::UP:
                        if (currentFrame == 0) rect = {840, 450, 50, 50};
                        else if (currentFrame == 1) rect = {840, 500, 50, 50};
                        else if (currentFrame == 2) rect = {840, 550, 50, 50};
                        break;

                    case logic::Direction::DOWN:
                        if (currentFrame == 0) rect = {840, 150, 50, 50};
                        else if (currentFrame == 1) rect = {840, 200, 50, 50};
                        else if (currentFrame == 2) rect = {840, 250, 50, 50};
                        break;

                    case logic::Direction::NONE:
                        // Wordt niet bereikt door if check hierboven
                        break;
                }

                sprite.setTextureRect(sf::IntRect(rect.x, rect.y, rect.width, rect.height));
            }
        } else {
            // PacMan beweegt niet - reset animation clock maar blijf op huidige sprite
            animationClock.restart();
        }

        // Rendering (altijd, ook als still)
        float centerX = pacManModel->getX();
        float centerY = pacManModel->getY();

        float pixelCenterX = camera->normalizedToPixelX(centerX);
        float pixelCenterY = camera->normalizedToPixelY(centerY);

        sprite.setOrigin(25.0f, 25.0f);
        sprite.setPosition(pixelCenterX, pixelCenterY);
        sprite.setScale(1.0f, 1.0f);

        window->draw(sprite);
    }
}
