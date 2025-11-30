#include "representation/views/WallView.h"

namespace representation {
    WallView::WallView(logic::WallModel* model, sf::RenderWindow* window, const Camera* camera)
            : EntityView(model, window, camera) {
        shape.setFillColor(sf::Color::Blue);  // Klassieke PacMan blauwe walls
        shape.setOutlineColor(sf::Color::Cyan);
        shape.setOutlineThickness(1.0f);
    }

    void WallView::draw() {
        // Model positie is CENTER, niet top-left
        float centerX = model->getX();
        float centerY = model->getY();
        float width = model->getWidth();
        float height = model->getHeight();

        // Bereken top-left corner
        float topLeftX = centerX - width / 2.0f;
        float topLeftY = centerY - height / 2.0f;

        // Convert naar pixels
        float pixelX = camera->normalizedToPixelX(topLeftX);
        float pixelY = camera->normalizedToPixelY(topLeftY);

        // Bereken size in pixels
        float pixelW = camera->normalizedToPixelX(topLeftX + width) - pixelX;
        float pixelH = camera->normalizedToPixelY(topLeftY + height) - pixelY;

        shape.setPosition(pixelX, pixelY);
        shape.setSize(sf::Vector2f(pixelW, pixelH));

        window->draw(shape);
    }
}