#ifndef PACMANGAME_PACMANVIEW_H
#define PACMANGAME_PACMANVIEW_H

#include <SFML/Graphics.hpp>
#include "representation/Camera.h"
#include "logic/entities/PacManModel.h"

namespace representation {
    struct SpriteRect {
        int x, y, width, height;
    };

    class PacManView {
    private:
        logic::PacManModel *model;

        sf::Texture texture;
        sf::Sprite sprite;

        float animationTimer;
        int frameIndex;
        const int frames[4] = {0, 1, 2, 1};

    public:
        PacManView(logic::PacManModel *model);

        void update(float deltaTime);

        void draw(sf::RenderWindow &window, const Camera &camera);
    };
}


#endif //PACMANGAME_PACMANVIEW_H
