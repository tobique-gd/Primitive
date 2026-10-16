#pragma once

#include "scene/entity.h"
namespace Primitive {
    struct EditorState {
        struct {
            entt::entity selected_entity = entt::null;
            int selected_tool = 0;
        } temporary;
    };
}
