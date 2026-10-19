#include "scene/main/node.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

Node::Node(const std::string& name)
    : m_name(name) {}

Node* Node::add_child(std::unique_ptr<Node> child) {
    if (!child || child.get() == this || child->m_parent != nullptr)
        return nullptr;

    for (Node* ancestor = this; ancestor; ancestor = ancestor->m_parent) {
        if (ancestor == child.get())
            return nullptr;
    }

    child->m_parent = this;

    Node* result = child.get();
    m_children.push_back(std::move(child));

    return result;
}

bool Node::remove_child(Node* child) {
    auto it = std::find_if(
        m_children.begin(),
        m_children.end(),
        [child](const std::unique_ptr<Node>& entry) {
            return entry.get() == child;
        }
    );

    if (it == m_children.end())
        return false;

    (*it)->m_parent = nullptr;
    m_children.erase(it);

    return true;
}


Node* Node::get_parent() const {
    return m_parent;
}

const std::vector<std::unique_ptr<Node>>& Node::get_children() const {
    return m_children;
}

Node* Node::find_child(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->m_name == name)
            return child.get();
    }

    return nullptr;
}

const std::string& Node::get_name() const {
    return m_name;
}

void Node::set_name(const std::string& name) {
    m_name = name;
}

const Primitive::GUID& Node::get_guid() const {
    return m_guid;
}
