
#include <string>
#include <unordered_map>

class Node {
    public:
        Node() = default;
        ~Node() = default;
        std::string name;
        Node* parent = nullptr;
        std::unordered_map<std::string, Node> children;


};
