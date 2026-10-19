#pragma once

#include "core/project/project_manager.h"
#include "scene/resources/3d/mesh.h"
#include "scene/scene.h"
#include "systems/rendering/backends/metal/vertex_data.h"
#include <filesystem>
#include <vector>

class aiMesh;

namespace Primitive {
    class EditorSceneFormatImporter {
        public:
            EditorSceneFormatImporter(std::shared_ptr<ProjectManager> project_manager);
            ~EditorSceneFormatImporter() = default;
            GUID import_scene(std::filesystem::path& path);
        private:
            Mesh build_meshlets(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);
            void extract_vertices(const aiMesh* mesh, std::vector<VertexData>& vertices);
            void extract_indices(const aiMesh* mesh, std::vector<uint32_t>& indices);

            std::shared_ptr<ProjectManager> m_project_manager;
    };
}
