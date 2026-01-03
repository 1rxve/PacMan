#ifndef PACMANGAME_ABSTRACTFACTORY_H
#define PACMANGAME_ABSTRACTFACTORY_H

#include "logic/entities/EntityModel.h"
#include "logic/entities/GhostModel.h"
#include "logic/patterns/Observer.h"
#include <memory>

namespace logic {
/**
 * Return type for entity factory methods.
 *
 * Pairs logic model (shared ownership) with its view observer (unique ownership).
 * Enables complete separation between logic and representation layers.
 */
struct EntityCreationResult {
    std::shared_ptr<EntityModel> model;
    std::unique_ptr<Observer> view;
};

/**
 * Abstract Factory pattern for creating game entities.
 *
 * Allows World (logic layer) to create entities without knowing about
 * SFML-specific view implementations. Concrete factory in representation
 * layer creates both model and view, attaching view as observer to model.
 *
 * This enforces complete separation: logic library compiles without SFML.
 */
class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;

    virtual EntityCreationResult createWall(float x, float y, float w, float h) = 0;

    virtual EntityCreationResult createPacMan(float x, float y, float w, float h, float speed) = 0;

    virtual EntityCreationResult createCoin(float x, float y, float w, float h) = 0;

    virtual EntityCreationResult createGhost(float x, float y, float w, float h, GhostType type, float spawnDelay) = 0;

    virtual EntityCreationResult createDoor(float x, float y, float w, float h) = 0;

    virtual EntityCreationResult createNoEntry(float x, float y, float w, float h) = 0;

    virtual EntityCreationResult createFruit(float x, float y, float w, float h) = 0;
};
} // namespace logic

#endif