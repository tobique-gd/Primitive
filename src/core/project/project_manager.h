#pragma once

#include "core/project/assets/asset_manager.h"
#include "core/project/project_file.h"
#include "core/project/scene/scene_manager.h"
#include <filesystem>
#include <memory>
#include <string>

namespace Primitive {
    class ProjectManager {
        public:
            ProjectManager() = default;
            ~ProjectManager() = default;

            bool new_project(const std::filesystem::path& folder_path);
            bool open_project(const std::filesystem::path& project_file_path);
            bool save_project();
            void close_project();

            inline bool is_project_open() const { return !m_project_folder.empty(); }
            inline std::shared_ptr<SceneManager> get_scene_manager() const { return m_scene_manager; }
            //inline std::shared_ptr<AssetManager> get_asset_manager() const { return m_asset_manager; }

            inline std::string get_project_name() { return m_project_folder.filename().string(); }
            inline std::filesystem::path get_project_folder() { return m_project_folder; }

        private:
            std::filesystem::path m_project_folder;
            ProjectFile m_project_file;

            std::shared_ptr<SceneManager> m_scene_manager = std::make_shared<SceneManager>();
            //std::shared_ptr<AssetManager> m_asset_manager = std::make_shared<AssetManager>();

    };
}
