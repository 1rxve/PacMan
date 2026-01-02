#include "representation/views/PacManView.h"
#include <iostream>

namespace representation {
PacManView::PacManView(logic::PacManModel* model, sf::RenderWindow* window, const Camera* camera,
                       std::shared_ptr<sf::Texture> sharedTexture)
    : EntityView(model, window, camera), pacManModel(model), texture(sharedTexture), animationTimer(0.0f),
      frameIndex(0) {

    // Gebruik shared texture (NIET opnieuw laden)
    sprite.setTexture(*texture);
    sprite.setTextureRect(sf::IntRect(840, 0, 50, 50));
}

void PacManView::draw() {
    if (pacManModel->getIsDying()) {
        renderDeathAnimation();
        return;
    }

    logic::Direction currentDir = pacManModel->getCurrentDirection();

    // Reset to full circle when stationary (including after respawn)
    if (currentDir == logic::Direction::NONE) {
        sprite.setTextureRect(sf::IntRect(840, 0, 50, 50));
        sprite.setOrigin(29.5f, 22.0f);
        sprite.setScale(1.15f, 1.15f);

        float centerX = pacManModel->getX();
        float centerY = pacManModel->getY();
        float pixelCenterX = camera->normalizedToPixelX(centerX);
        float pixelCenterY = camera->normalizedToPixelY(centerY);

        sprite.setPosition(pixelCenterX, pixelCenterY);
        window->draw(sprite);
        return;
    }

    if (currentDir != logic::Direction::NONE) {
        float deltaTime = animationClock.restart().asSeconds();
        animationTimer += deltaTime;

        if (animationTimer >= 0.1f) {
            frameIndex = (frameIndex + 1) % 4;
            animationTimer = 0.0f;

            int currentFrame = frames[frameIndex];
            SpriteRect rect = {840, 0, 50, 50};

            switch (currentDir) {
            case logic::Direction::RIGHT:
                if (currentFrame == 0)
                    rect = {840, 0, 50, 50};
                else if (currentFrame == 1)
                    rect = {840, 50, 50, 50};
                else if (currentFrame == 2)
                    rect = {840, 100, 50, 50};
                break;

            case logic::Direction::LEFT:
                if (currentFrame == 0)
                    rect = {840, 300, 50, 50};
                else if (currentFrame == 1)
                    rect = {840, 350, 50, 50};
                else if (currentFrame == 2)
                    rect = {840, 400, 50, 50};
                break;

            case logic::Direction::UP:
                if (currentFrame == 0)
                    rect = {840, 450, 50, 50};
                else if (currentFrame == 1)
                    rect = {840, 500, 50, 50};
                else if (currentFrame == 2)
                    rect = {840, 550, 50, 50};
                break;

            case logic::Direction::DOWN:
                if (currentFrame == 0)
                    rect = {840, 150, 50, 50};
                else if (currentFrame == 1)
                    rect = {840, 200, 50, 50};
                else if (currentFrame == 2)
                    rect = {840, 250, 50, 50};
                break;

            case logic::Direction::NONE:
                break;
            }

            sprite.setTextureRect(sf::IntRect(rect.x, rect.y, rect.width, rect.height));
        }
    } else {
        animationClock.restart();
    }

    // Rendering
    float centerX = pacManModel->getX();
    float centerY = pacManModel->getY();

    float pixelCenterX = camera->normalizedToPixelX(centerX);
    float pixelCenterY = camera->normalizedToPixelY(centerY);

    // Sprite rendering met GECORRIGEERDE origin
    sprite.setOrigin(29.5f, 22.0f); // Handmatig bepaald voor correcte centering
    sprite.setPosition(pixelCenterX, pixelCenterY);
    sprite.setScale(1.15f, 1.15f);
    window->draw(sprite);

    // DEBUG VISUALISATIE (optioneel)
    if (showDebugVisualization) {
        // Groen vierkant = sprite texture bounds
        sf::RectangleShape debugBox(sf::Vector2f(50.0f, 50.0f));
        debugBox.setFillColor(sf::Color::Transparent);
        debugBox.setOutlineColor(sf::Color::Green);
        debugBox.setOutlineThickness(1.0f);
        debugBox.setOrigin(25.0f, 25.0f); // Zelfde origin als sprite
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

void PacManView::renderDeathAnimation() {
    float deathTimer = pacManModel->getDeathTimer();
    const float DEATH_ANIMATION_DURATION = 2.0f;

    int totalFrames = 11;
    int frameIndex = static_cast<int>((deathTimer / DEATH_ANIMATION_DURATION) * totalFrames);

    if (frameIndex >= totalFrames) {
        return;
    }

    int deathSpriteX = 350;
    int deathSpriteY = 0 + (frameIndex * 50);

    sprite.setTextureRect(sf::IntRect(deathSpriteX, deathSpriteY, 50, 50));

    float centerX = pacManModel->getX();
    float centerY = pacManModel->getY();

    float pixelCenterX = camera->normalizedToPixelX(centerX);
    float pixelCenterY = camera->normalizedToPixelY(centerY);

    // Different origin for last frame
    if (frameIndex == 10) {
        sprite.setOrigin(20.0f, 27.0f);
    } else {
        sprite.setOrigin(20.0f, 20.0f);
    }

    sprite.setPosition(pixelCenterX, pixelCenterY);
    sprite.setScale(1.15f, 1.15f);

    window->draw(sprite);
}
} // namespace representation