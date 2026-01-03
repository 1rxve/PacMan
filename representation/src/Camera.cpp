#include "representation/Camera.h"

namespace representation {
Camera::Camera(float windowWidth, float windowHeight, float sidebarWidth)
    : windowWidth(windowWidth), windowHeight(windowHeight), sidebarWidth(sidebarWidth) {}

// Converts [-1, 1] → pixel X with game area centered between left/right sidebars
// Formula: leftSidebar + (normalized + 1) * (gameWidth / 2)
float Camera::normalizedToPixelX(float normalizedX) const {
    float gameAreaWidth = windowWidth - (2 * sidebarWidth);
    return sidebarWidth + (normalizedX + 1.0f) * (gameAreaWidth / 2.0f);
}

// Converts [-1, 1] → pixel Y using full window height
// Formula: (normalized + 1) * (height / 2)
float Camera::normalizedToPixelY(float normalizedY) const {
    return (normalizedY + 1.0f) * (windowHeight / 2.0f);
}
} // namespace representation