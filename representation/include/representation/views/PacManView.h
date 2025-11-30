#ifndef PACMANGAME_PACMANVIEW_H
#define PACMANGAME_PACMANVIEW_H

#include "EntityView.h"
#include "logic/entities/PacManModel.h"

namespace representation {
    struct SpriteRect {
        int x;
        int y;
        int width;
        int height;
    };

    class PacManView : public EntityView{
    private:
        logic::PacManModel* pacManModel;

        sf::Texture texture;
        sf::Sprite sprite;

        float animationTimer;
        int frameIndex;
        const int frames[4] = {0, 1, 2, 1};

        sf::Clock animationClock;

    public:
        PacManView(logic::PacManModel* model, sf::RenderWindow* window, const Camera* camera);

        void draw() override;
    };
}


#endif //PACMANGAME_PACMANVIEW_H
