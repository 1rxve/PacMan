#ifndef PACMANGAME_COINVIEW_H
#define PACMANGAME_COINVIEW_H

#include "EntityView.h"
#include "logic/entities/CoinModel.h"
#include <memory> // ← Voor shared_ptr

namespace representation {
class CoinView : public EntityView {
private:
    logic::CoinModel* coinModel;
    std::shared_ptr<sf::Texture> texture; // ← SHARED_PTR
    sf::Sprite sprite;

public:
    CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera,
             std::shared_ptr<sf::Texture> sharedTexture); // ← SHARED_PTR parameter

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_COINVIEW_H