#ifndef PACMANGAME_NOENTRYMODEL_H
#define PACMANGAME_NOENTRYMODEL_H

#include "EntityModel.h"

namespace logic {
    class NoEntryModel : public EntityModel {
    public:
        NoEntryModel(float x, float y, float width, float height);

        void update(float deltaTime) override;

        bool isNoEntry() const override { return true; }
    };
}

#endif // PACMANGAME_NOENTRYMODEL_H