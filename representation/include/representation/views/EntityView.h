#ifndef PACMANGAME_ENTITYVIEW_H
#define PACMANGAME_ENTITYVIEW_H

#include <SFML/Graphics.hpp>
#include "logic/patterns/Observer.h"
#include "logic/entities/EntityModel.h"
#include "representation/Camera.h"

namespace representation {
    class EntityView : public logic::Observer {
    protected:
        logic::EntityModel* model;
        sf::RenderWindow* window;
        const Camera* camera;

    public:
        EntityView(logic::EntityModel* model, sf::RenderWindow* window, const Camera* camera);  // <- Camera parameter
        virtual ~EntityView();

        void onNotify() override;
        virtual void draw() = 0;
    };
}



#endif //PACMANGAME_ENTITYVIEW_H
