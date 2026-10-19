#pragma once

#include "scene/resources/main/resource.h"
#include <vector>
#include "meshoptimizer.h"
#include "systems/rendering/backends/metal/vertex_data.h"

class Mesh : public Resource {
    public:
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;

        std::vector<meshopt_Meshlet> meshlets;
        std::vector<uint32_t> meshlet_vertices;
        std::vector<unsigned char> meshlet_triangles;









};
