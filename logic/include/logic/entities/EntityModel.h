#ifndef PACMANGAME_ENTITYMODEL_H
#define PACMANGAME_ENTITYMODEL_H

namespace logic {

class EntityModel {
protected:
    float x;
    float y;
    float width;
    float height;

public:
    EntityModel(float x, float y, float width, float height);

    virtual ~EntityModel();

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    void setPosition(float newX, float newY);

    virtual void update(float deltaTime) = 0;

    // Collision detection
    bool intersects(const EntityModel& other) const;


};

}
#endif //PACMANGAME_ENTITYMODEL_H