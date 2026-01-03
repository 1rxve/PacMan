#include "representation/views/WallView.h"
#include <iostream>

namespace representation {
WallView::WallView(logic::WallModel* model, sf::RenderWindow* window, const Camera* camera)
    : EntityView(model, window, camera) {
    shape.setFillColor(sf::Color::Blue); // Classic PacMan blue maze walls
}

void WallView::draw() {
    // Convert center-based normalized coordinates to top-left pixel coordinates
    float centerX = model->getX();
    float centerY = model->getY();
    float width = model->getWidth();
    float height = model->getHeight();

    float topLeftX = centerX - width / 2.0f;
    float topLeftY = centerY - height / 2.0f;

    float pixelX = camera->normalizedToPixelX(topLeftX);
    float pixelY = camera->normalizedToPixelY(topLeftY);

    float bottomRightX = centerX + width / 2.0f;
    float bottomRightY = centerY + height / 2.0f;

    float pixelRightX = camera->normalizedToPixelX(bottomRightX);
    float pixelBottomY = camera->normalizedToPixelY(bottomRightY);

    float pixelW = pixelRightX - pixelX;
    float pixelH = pixelBottomY - pixelY;

    shape.setPosition(pixelX, pixelY);
    shape.setSize(sf::Vector2f(pixelW, pixelH));

    window->draw(shape);
}
} // namespace representation