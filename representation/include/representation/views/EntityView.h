#ifndef PACMANGAME_ENTITYVIEW_H
#define PACMANGAME_ENTITYVIEW_H

#include "logic/entities/EntityModel.h"
#include "logic/patterns/Observer.h"
#include "representation/Camera.h"
#include <SFML/Graphics.hpp>

namespace representation {
/**
 * Abstract base class for SFML entity rendering (View in MVC pattern).
 *
 * Implements Observer pattern to receive update notifications from EntityModel:
 * - Auto-attaches to model on construction (via model->attach(this))
 * - Auto-detaches on destruction (prevents dangling observer)
 * - onNotify() calls draw() to render current model state
 *
 * Subclasses implement draw() with SFML-specific rendering logic.
 *
 * Protected members allow subclasses direct access to:
 * - model: Logic state (raw pointer, not owned)
 * - window: SFML render target
 * - camera: Normalized → pixel coordinate conversion
 */
class EntityView : public logic::Observer {
protected:
    logic::EntityModel* model;
    sf::RenderWindow* window;
    const Camera* camera;

public:
    EntityView(logic::EntityModel* model, sf::RenderWindow* window, const Camera* camera);
    virtual ~EntityView();

    /**
     * Observer callback - invoked by EntityModel::notify() on state changes.
     * Calls draw() to render current model state to window.
     */
    void onNotify() override;

    virtual void draw() = 0;
};
} // namespace representation

#endif // PACMANGAME_ENTITYVIEW_H