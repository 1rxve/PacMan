#ifndef PACMANGAME_CAMERA_H
#define PACMANGAME_CAMERA_H

namespace representation {
    class Camera {
    private:
        float windowWidth;
        float windowHeight;
        float sidebarWidth;  // NEW

    public:
        Camera(float windowWidth, float windowHeight, float sidebarWidth);

        float normalizedToPixelX(float normalizedX) const;
        float normalizedToPixelY(float normalizedY) const;

        float getSidebarWidth() const { return sidebarWidth; }
        float getGameAreaWidth() const { return windowWidth - sidebarWidth; }
    };
}

#endif