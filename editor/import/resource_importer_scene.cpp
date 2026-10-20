#include "editor/import/resource_importer_scene.h"
#include "assimp/mesh.h"

#include "scene/3d/geometry_instance.h"
#include "scene/resources/3d/mesh.h"
#include <cstdint>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <span>
#include <vector>

namespace Primitive {
    EditorSceneFormatImporter::EditorSceneFormatImporter(std::shared_ptr<ProjectManager> project_manager) : m_project_manager(project_manager) {};

    GUID EditorSceneFormatImporter::import_scene(std::filesystem::path& path) {
        Assimp::Importer importer;

        const aiScene* imported_scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_FlipWindingOrder);

        if (nullptr == imported_scene) {
            return GUID::INVALID;
        }
        std::span<const aiMesh* const> meshes(imported_scene->mMeshes, imported_scene->mNumMeshes);

        GUID s_guid = m_project_manager->get_scene_manager()->create_scene();
        auto scene = m_project_manager->get_scene_manager()->find_scene(s_guid);

        for (const aiMesh* mesh : meshes) {
            std::vector<VertexData> vertices;
            std::vector<uint32_t> indices;

            extract_vertices(mesh, vertices);

            extract_indices(mesh, indices);

            Mesh m = build_meshlets(vertices, indices);

            auto geo_instance = std::make_unique<GeometryInstance>();
            geo_instance->mesh = m;

            scene->get_root()->add_child(std::move(geo_instance));

        }

        return s_guid;

    };

    void EditorSceneFormatImporter::extract_indices(const aiMesh* mesh, std::vector<uint32_t>& indices) {
        for (uint32_t  i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];

            for (uint32_t j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    void EditorSceneFormatImporter::extract_vertices(const aiMesh* mesh, std::vector<VertexData>& vertices) {
        vertices.reserve(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            VertexData vertex;

            vertex.position[0] = mesh->mVertices[i].x;
            vertex.position[1] = mesh->mVertices[i].y;
            vertex.position[2] = mesh->mVertices[i].z;
            vertex.position[3] = 1.0;

            if (mesh->HasNormals()) {
                vertex.normal[0] = mesh->mNormals[i].x;
                vertex.normal[1] = mesh->mNormals[i].y;
                vertex.normal[2] = mesh->mNormals[i].z;
            } else {
                vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0.0f;
            }

            if (mesh->mTextureCoords[0]) {
                vertex.texture_coordinate[0] = mesh->mTextureCoords[0][i].x;
                vertex.texture_coordinate[1] = mesh->mTextureCoords[0][i].y;
            } else {
                vertex.texture_coordinate[0] = vertex.texture_coordinate[1] = 0.0f;
            }

            vertices.push_back(vertex);
        }
    }


    Mesh EditorSceneFormatImporter::build_meshlets(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices) {
        const size_t max_vertices = 64;
        const size_t max_triangles = 64;
        const float cone_weight = 0.5f;

        size_t max_meshlets = meshopt_buildMeshletsBound(indices.size(), max_vertices, max_triangles);
        std::vector<meshopt_Meshlet> meshlets(max_meshlets);

        std::vector<unsigned int> meshlet_vertices(indices.size());
        std::vector<unsigned char> meshlet_triangles(indices.size());

        size_t meshlet_count = meshopt_buildMeshlets(
            meshlets.data(),
            meshlet_vertices.data(),
            meshlet_triangles.data(),
            indices.data(),
            indices.size(),
            &vertices[0].position[0],
            vertices.size(),
            sizeof(VertexData),
            max_vertices,
            max_triangles,
            cone_weight
        );

        if (meshlet_count == 0) return Mesh();

        const meshopt_Meshlet& last = meshlets[meshlet_count - 1];

        meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
        meshlet_triangles.resize(last.triangle_offset + last.triangle_count * 3);
        meshlets.resize(meshlet_count);

        meshopt_optimizeMeshlet(&meshlet_vertices[last.vertex_offset], &meshlet_triangles[last.triangle_offset], last.triangle_count, last.vertex_count);

        Mesh m = Mesh();
        m.meshlets = meshlets;
        m.meshlet_vertices = meshlet_vertices;
        m.meshlet_triangles = meshlet_triangles;
        m.vertices = vertices;
        m.indices = indices;

        return m;
    }

}
