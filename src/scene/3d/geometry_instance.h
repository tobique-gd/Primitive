#include "scene/3d/visual_instance.h"
#include "scene/resources/3d/mesh.h"

class GeometryInstance : public VisualInstance {
    public:
        GeometryInstance(const std::string& name = "GeometryInstance") : VisualInstance(name) {}

        Mesh mesh;

};
