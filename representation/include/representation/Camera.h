#ifndef PACMANGAME_CAMERA_H
#define PACMANGAME_CAMERA_H

namespace representation {
/**
 * Converts normalized game coordinates [-1, 1] to pixel coordinates for rendering.
 *
 * Enables resolution-independent game logic by separating world space from screen space.
 * Logic library operates in normalized coordinates, representation uses pixels.
 *
 * Supports sidebar regions for UI elements (score, lives) via sidebarWidth parameter.
 * Game area is centered between left and right sidebars.
 */
class Camera {
private:
    float windowWidth;
    float windowHeight;
    float sidebarWidth;

public:
    Camera(float windowWidth, float windowHeight, float sidebarWidth);

    /**
     * Converts normalized X coordinate [-1, 1] to pixel X coordinate.
     *
     * Formula: sidebarWidth + (normalizedX + 1) * (gameAreaWidth / 2)
     * Centers game area between left and right sidebars.
     */
    float normalizedToPixelX(float normalizedX) const;

    /**
     * Converts normalized Y coordinate [-1, 1] to pixel Y coordinate.
     *
     * Formula: (normalizedY + 1) * (windowHeight / 2)
     * No vertical sidebars - full height used for game area.
     */
    float normalizedToPixelY(float normalizedY) const;

    float getSidebarWidth() const { return sidebarWidth; }

    float getGameAreaWidth() const { return windowWidth - sidebarWidth; }
};
} // namespace representation

#endif