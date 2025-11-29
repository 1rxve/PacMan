#include "representation/Camera.h"

namespace representation {
    Camera::Camera(float windowWidth, float windowHeight)
        : windowWidth(windowWidth), windowHeight(windowHeight) {
    }

    float Camera::normalizedToPixelX(float normalizedX) const {
        return (normalizedX + 1.0f) * (windowWidth / 2.0f);
    }

    float Camera::normalizedToPixelY(float normalizedY) const {
        return (normalizedY + 1.0f) * (windowHeight / 2.0f);
    }
}
