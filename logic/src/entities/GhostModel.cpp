#include "logic/entities/GhostModel.h"

namespace logic {
    GhostModel::GhostModel(float x, float y, float width, float height,
                           GhostType type, float spawnDelay)
            : EntityModel(x, y, width, height),
              type(type),
              state(GhostState::SPAWNING),
              spawnDelay(spawnDelay),
              spawnTimer(0.0f),
              speed(0.5f) {
    }

    void GhostModel::update(float deltaTime) {
        // SPAWNING state - wacht op delay
        if (state == GhostState::SPAWNING) {
            spawnTimer += deltaTime;

            if (spawnTimer >= spawnDelay) {
                state = GhostState::CHASING;
                // Later: start AI movement
            }

            notify();  // Update view
            return;    // Geen movement tijdens spawning
        }

        // CHASING state - later: AI movement
        if (state == GhostState::CHASING) {
            // TODO: AI logic hier
            notify();
            return;
        }

        // Other states later
        notify();
    }
}