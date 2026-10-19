#pragma once

#include "core/guid.h"

#include <memory>
#include <string>
#include <vector>

class Node {
    public:
        explicit Node(const std::string& name = "Node");
        virtual ~Node() = default;

        Node* add_child(std::unique_ptr<Node> child);
        bool remove_child(Node* child);

        Node* get_parent() const;
        const std::vector<std::unique_ptr<Node>>& get_children() const;

        Node* find_child(const std::string& name) const;

        const std::string& get_name() const;
        void set_name(const std::string& name);

        const Primitive::GUID& get_guid() const;

    private:
        Primitive::GUID m_guid;
        std::string m_name;
        Node* m_parent = nullptr;
        std::vector<std::unique_ptr<Node>> m_children;
    };
