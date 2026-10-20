#pragma once

#include "glm/ext/vector_float3.hpp"
#include "scene/resources/main/resource.h"

class AABB : public Resource {
    public:
        glm::vec3 start = glm::vec3(0.0); // bottom left corner of the box
        glm::vec3 end = glm::vec3(0.0); //top right corner of the box
        glm::vec3 size = glm::vec3(0.0);

};
