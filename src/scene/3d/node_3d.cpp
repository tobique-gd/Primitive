#include "scene/3d/node_3d.h"
#include "glm/ext/matrix_transform.hpp"

glm::mat4x4 Node3D::get_transform() {
    auto t = glm::translate(glm::identity<glm::mat4x4>(), position);
    t = glm::rotate(t, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    t = glm::rotate(t, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    t = glm::rotate(t, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    t = glm::scale(t, scale);

    return t;
}
