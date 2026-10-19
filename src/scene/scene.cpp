#include "scene/scene.h"

#include <utility>

namespace Primitive {
    Scene::Scene(const std::string& name)
        : name(name),
          m_root(std::make_unique<Node>("Root")) {}

    Node* Scene::create_node(const std::string& name) {
        auto node = std::make_unique<Node>(name);
        return m_root->add_child(std::move(node));
    }

    bool Scene::destroy_node(Node* node) {
        if (!node || node == m_root.get())
            return false;

        Node* parent = node->get_parent();

        if (!parent)
            return false;

        return parent->remove_child(node);
    }

    Node* Scene::get_root() const {
        return m_root.get();
    }
}
