#ifndef PACMANGAME_CONCRETEFACTORY_H
#define PACMANGAME_CONCRETEFACTORY_H

#include "logic/patterns/AbstractFactory.h"
#include "representation/Camera.h"
#include <SFML/Graphics.hpp>

namespace representation {
/**
 * Concrete implementation of AbstractFactory for creating SFML-coupled entities.
 *
 * Enables World (logic layer) to create entities without knowing about SFML:
 * - Creates EntityModel (shared_ptr, logic layer)
 * - Creates corresponding EntityView (unique_ptr, representation layer)
 * - Auto-attaches view as observer to model
 * - Returns both via EntityCreationResult struct
 *
 * Texture management:
 * - sharedTexture: Main sprite sheet (pacman_sprites.png) shared across all views
 * - doorTexture: Separate texture for door sprite (door_sprite.png)
 * - Loaded once in constructor, shared via shared_ptr to avoid redundant loads
 *
 * This enforces complete separation: logic library compiles without SFML.
 */
class ConcreteFactory : public logic::AbstractFactory {
private:
    sf::RenderWindow* window;
    const Camera* camera;
    std::shared_ptr<sf::Texture> sharedTexture;
    std::shared_ptr<sf::Texture> doorTexture;

public:
    ConcreteFactory(sf::RenderWindow* window, const Camera* camera);

    logic::EntityCreationResult createWall(float x, float y, float w, float h) override;

    logic::EntityCreationResult createPacMan(float x, float y, float w, float h, float speed) override;

    logic::EntityCreationResult createCoin(float x, float y, float w, float h) override;

    logic::EntityCreationResult createGhost(float x, float y, float w, float h, logic::GhostType type,
                                            float spawnDelay) override;

    logic::EntityCreationResult createDoor(float x, float y, float w, float h) override;

    logic::EntityCreationResult createNoEntry(float x, float y, float w, float h) override;

    logic::EntityCreationResult createFruit(float x, float y, float w, float h) override;
};
} // namespace representation

#endif // PACMANGAME_CONCRETEFACTORY_H