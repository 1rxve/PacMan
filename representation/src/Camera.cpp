#include "representation/Camera.h"

namespace representation {
    Camera::Camera(float windowWidth, float windowHeight, float sidebarWidth)
            : windowWidth(windowWidth),
              windowHeight(windowHeight),
              sidebarWidth(sidebarWidth) {
    }

    float Camera::normalizedToPixelX(float normalizedX) const {
        // Game centered between left and right sidebars
        float gameAreaWidth = windowWidth - (2 * sidebarWidth);  // ← CHANGE: 2× sidebar
        return sidebarWidth + (normalizedX + 1.0f) * (gameAreaWidth / 2.0f);
    }

    float Camera::normalizedToPixelY(float normalizedY) const {
        return (normalizedY + 1.0f) * (windowHeight / 2.0f);
    }
}