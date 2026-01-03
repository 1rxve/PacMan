#ifndef PACMANGAME_COINVIEW_H
#define PACMANGAME_COINVIEW_H

#include "EntityView.h"
#include "logic/entities/CoinModel.h"
#include <memory>

namespace representation {
/**
 * SFML view for rendering coin collectibles.
 *
 * Shares texture with other views to avoid redundant texture loads.
 * Automatically hides when CoinModel::isCollected() returns true.
 *
 * Sprite: 6x6 pixel region from shared sprite sheet.
 */
class CoinView : public EntityView {
private:
    logic::CoinModel* coinModel;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

public:
    CoinView(logic::CoinModel* model, sf::RenderWindow* window, const Camera* camera,
             std::shared_ptr<sf::Texture> sharedTexture);

    void draw() override;
};
} // namespace representation

#endif // PACMANGAME_COINVIEW_H