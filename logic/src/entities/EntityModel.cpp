#include "logic/entities/EntityModel.h"
namespace logic {

EntityModel::EntityModel(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {
}

EntityModel::~EntityModel() {

}

float EntityModel::getX() const {
    return x;
}

float EntityModel::getY() const {
    return y;
}

float EntityModel::getWidth() const {
    return width;
}

float EntityModel::getHeight() const {
    return height;
}


void EntityModel::setPosition(float newX, float newY) {
    x = newX;
    y = newY;
}

// Collision detecting
bool EntityModel::intersects(const EntityModel& other) const {
    float left1 = x - width / 2.0f;
    float right1 = x + width / 2.0f;
    float top1 = y - height / 2.0f;
    float bottom1 = y + height / 2.0f;

    float left2 = other.x - other.width / 2.0f;
    float right2 = other.x + other.width / 2.0f;
    float top2 = other.y - other.height / 2.0f;
    float bottom2 = other.y + other.height / 2.0f;

    return !(right1 < left2 || left1 > right2 ||
             bottom1 < top2 || top1 > bottom2);
}



}