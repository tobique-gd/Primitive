#pragma once

#include "core/guid.h"

namespace Primitive {
    struct IDComponent {
        IDComponent() = default;
        IDComponent(GUID guid) : guid(guid) {};
        GUID guid;
    };

    struct TagComponent {
        TagComponent() = default;
        TagComponent(const std::string& tag) : tag(tag) {};

        std::string tag;
    };
}
