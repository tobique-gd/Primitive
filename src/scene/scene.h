#pragma once

#include "core/guid.h"
#include "scene/main/node.h"

#include <memory>
#include <string>

namespace Primitive {
    class Scene {
    public:
        explicit Scene(const std::string& name = "Scene");

        Node* create_node(const std::string& name = "Node");
        bool destroy_node(Node* node);

        Node* get_root() const;

        GUID guid;
        std::string name;

    private:
        std::unique_ptr<Node> m_root;
    };
}
