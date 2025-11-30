#include "representation/views/WallView.h"

namespace representation {
    WallView::WallView(logic::WallModel* model, sf::RenderWindow* window, const Camera* camera)
            : EntityView(model, window, camera) {
        shape.setFillColor(sf::Color::Blue);  // Klassieke PacMan blauwe walls
        shape.setOutlineColor(sf::Color::Cyan);
        shape.setOutlineThickness(1.0f);
    }

    void WallView::draw() {
        // Bereken pixel positie via camera
        float pixelX = camera->normalizedToPixelX(model->getX());
        float pixelY = camera->normalizedToPixelY(model->getY());

        // Bereken pixel size
        float pixelW = camera->normalizedToPixelX(model->getX() + model->getWidth()) - pixelX;
        float pixelH = camera->normalizedToPixelY(model->getY() + model->getHeight()) - pixelY;

        // Update shape
        shape.setPosition(pixelX, pixelY);
        shape.setSize(sf::Vector2f(pixelW, pixelH));

        // Teken
        window->draw(shape);
    }
}