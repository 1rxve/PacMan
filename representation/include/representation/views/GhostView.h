#ifndef PACMANGAME_GHOSTVIEW_H
#define PACMANGAME_GHOSTVIEW_H

#include <memory>
#include "EntityView.h"
#include "logic/entities/GhostModel.h"

namespace representation {
    class GhostView : public EntityView {
    private:
        logic::GhostModel* ghostModel;
        std::shared_ptr<sf::Texture> texture;
        sf::Sprite sprite;

        bool showDebugVisualization;  // ← ADD

        float animationTimer;
        int frameIndex;

    public:
        GhostView(logic::GhostModel* model, sf::RenderWindow* window,
                  const Camera* camera, std::shared_ptr<sf::Texture> sharedTexture);

        void draw() override;
    };
}


#endif //PACMANGAME_GHOSTVIEW_H
