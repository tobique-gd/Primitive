#pragma once
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "scene/main/node.h"

class Node3D : public Node {
    public:
        explicit Node3D(const std::string& name = "Node3D") : Node(name) {}

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(0.0f, 0.0f, 0.0f);

        glm::mat4x4 get_transform();

    private:
        glm::mat4x4 transform;
};
