#pragma once

#include "core/guid.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <glm/glm.hpp>

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

    struct CameraComponent {
        CameraComponent() = default;
        CameraComponent(glm::mat4x4 transform) : transform(transform) {};

        glm::mat4x4 transform = glm::mat4x4();
    };
}
