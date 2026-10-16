#include "scene/scene_manager.h"
#include <memory>


namespace Primitive {
    GUID SceneManager::create_scene(const std::string& name) {
        std::shared_ptr<Scene> new_scene = std::make_shared<Scene>();
        new_scene->name = name;
        m_scenes[new_scene->guid] = new_scene;
        return new_scene->guid;
    }

    void SceneManager::delete_scene(GUID guid) {
        if (m_open_scene_guid == guid) {
			m_open_scene_guid = GUID::INVALID;
		}

        m_scenes.erase(guid);
    }

    bool SceneManager::set_open_scene_guid(GUID guid) {
        auto scene = find_scene(guid);

        if (!scene && guid != GUID::INVALID) {
            return false;
        }

        m_open_scene_guid = guid;
        return true;

    }

    GUID SceneManager::get_open_scene_guid() const {
        return m_open_scene_guid;
    }

    std::shared_ptr<Scene> SceneManager::get_open_scene() const {
        if (m_open_scene_guid == GUID::INVALID) {
            return nullptr;
        }

        auto it = m_scenes.find(m_open_scene_guid);

        if (it == m_scenes.end()) {
            return nullptr;
        }

        return it->second;
    }



}
