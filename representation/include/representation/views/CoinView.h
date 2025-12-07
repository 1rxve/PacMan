#ifndef PACMANGAME_COINVIEW_H
#define PACMANGAME_COINVIEW_H

#include "EntityView.h"
#include "logic/entities/CoinModel.h"

namespace representation {
    class CoinView : public EntityView {
    private:
        logic::CoinModel* coinModel;
        sf::Texture texture;
        sf::Sprite sprite;

    public:
        CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera);

        void draw() override;
    };
}

#endif // PACMANGAME_COINVIEW_H