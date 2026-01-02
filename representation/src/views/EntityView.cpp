#include "representation/views/EntityView.h"

namespace representation {
EntityView::EntityView(logic::EntityModel* model, sf::RenderWindow* window, const Camera* camera)
    : model(model), window(window), camera(camera) {
    model->attach(this);
}

EntityView::~EntityView() { model->detach(this); }

void EntityView::onNotify() {
    draw(); // Roept subclass draw() aan (zonder parameter, gebruikt camera member)
}
} // namespace representation
