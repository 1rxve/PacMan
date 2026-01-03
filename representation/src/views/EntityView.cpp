#include "representation/views/EntityView.h"

namespace representation {
// Auto-attach to model on construction (Observer pattern registration)
EntityView::EntityView(logic::EntityModel* model, sf::RenderWindow* window, const Camera* camera)
    : model(model), window(window), camera(camera) {
    model->attach(this);
}

// Auto-detach on destruction (prevents dangling observer pointer)
EntityView::~EntityView() {
    model->detach(this);
}

// Observer callback: delegates to subclass draw() implementation (polymorphic dispatch)
void EntityView::onNotify() {
    draw();
}
} // namespace representation