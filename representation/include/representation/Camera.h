#ifndef PACMANGAME_CAMERA_H
#define PACMANGAME_CAMERA_H

namespace representation {
    class Camera {
    private:
        float windowWidth;
        float windowHeight;

    public:
        Camera(float windowWidth, float windowHeight);

        float normalizedToPixelX(float normalizedX) const;

        float normalizedToPixelY(float normalizedY) const;
    };
}


#endif //PACMANGAME_CAMERA_H
