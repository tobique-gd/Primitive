#include "scene.h"
#include "core/project/scene/components.h"
#include "core/project/scene/entity.h"
#include <string>

namespace Primitive {
    EntityHandle Scene::create_entity(const std::string& name) {
        entt::entity entity_id = m_registry->create();
        EntityHandle entity(entity_id, m_registry);
        entity.get_or_add_component<IDComponent>();
        entity.get_or_add_component<TagComponent>(name);
        return entity;

    };

    void Scene::destroy_entity(EntityHandle entity) {
        m_registry->destroy(entity.get_entt_id());
    }
}
