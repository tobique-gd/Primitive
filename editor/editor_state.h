#pragma once

#include "scene/main/node.h"
namespace Primitive {
    struct EditorState {
        struct {
            Node* selected_node = nullptr;
            int selected_tool = 0;
        } temporary;
    };
}
