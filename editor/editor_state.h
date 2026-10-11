#pragma once

#include "core/project/scene/entity.h"
namespace Primitive {
    struct EditorState {
        struct {
            entt::entity selected_entity = entt::null;

        } temporary;
    };
}
