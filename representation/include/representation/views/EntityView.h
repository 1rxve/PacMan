#ifndef PACMANGAME_ENTITYVIEW_H
#define PACMANGAME_ENTITYVIEW_H

#include "logic/entities/EntityModel.h"
#include "logic/patterns/Observer.h"
#include "representation/Camera.h"
#include <SFML/Graphics.hpp>

namespace representation {
class EntityView : public logic::Observer {
protected:
    logic::EntityModel* model;
    sf::RenderWindow* window;
    const Camera* camera;

public:
    EntityView(logic::EntityModel* model, sf::RenderWindow* window, const Camera* camera); // <- Camera parameter
    virtual ~EntityView();

    void onNotify() override;
    virtual void draw() = 0;
};
} // namespace representation

#endif // PACMANGAME_ENTITYVIEW_H
