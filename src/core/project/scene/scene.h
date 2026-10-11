#pragma once

#include "core/guid.h"
#include "core/project/scene/entity.h"
#include "thirdparty/entt/entt.hpp"

namespace Primitive {
    class Scene {
        public:
            explicit Scene() { m_registry = new entt::registry(); }
            ~Scene() { delete m_registry; }

            EntityHandle create_entity(const std::string& name = "Empty Entity");
            void destroy_entity(EntityHandle entity);

            GUID guid;
            std::string name;

            inline entt::registry* get_registry() const { return m_registry; }


        private:
            entt::registry* m_registry;
    };
}
