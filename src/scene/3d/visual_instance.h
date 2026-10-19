#include "scene/3d/node_3d.h"

class VisualInstance : public Node3D {
    public:
        explicit VisualInstance(const std::string& name = "VisualInstance") : Node3D(name) {}
};
