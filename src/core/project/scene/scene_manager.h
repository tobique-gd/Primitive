#pragma once

#include "core/guid.h"
#include "core/project/scene/scene.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace Primitive {
    class SceneManager {
        public:
            SceneManager() = default;
            ~SceneManager() = default;

            GUID create_scene(const std::string& name = "Empty Scene");
            void delete_scene(GUID guid);

            bool set_open_scene_guid(GUID guid);
            GUID get_open_scene_guid() const;
            std::shared_ptr<Scene> get_open_scene() const;

            inline std::shared_ptr<Scene> find_scene(GUID guid) const {
    			if (guid == GUID::INVALID) {
    				return nullptr;
    			}
    			if (auto it = m_scenes.find(guid); it != m_scenes.end()){
    				return it->second;
    			}
    			return nullptr;
    		}

        private:
            GUID m_open_scene_guid = GUID::INVALID;

            std::unordered_map<GUID, std::shared_ptr<Scene>> m_scenes;

    };
}
