#pragma once

#include "thirdparty/entt/entt.hpp"


// Taken inspiration from Laura engine
// https://github.com/jakubg05/Laura/blob/main/Laura/src/Project/Scene/Entity.h

namespace Primitive {

	class EntityHandle {
	public:
		EntityHandle() = default;
		~EntityHandle() = default;

		EntityHandle(entt::entity entity, entt::registry* registry)
			: m_entity_id(entity), m_registry(registry) {
		}

		template<typename T, typename... Args>
		T& get_or_add_component(Args&&... args) const {
			if (has_component<T>()) { return get_component<T>(); }
			return m_registry->emplace<T>(m_entity_id, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get_component() const {
			return m_registry->get<T>(m_entity_id);
		}

		template<typename T>
		bool has_component() const {
			return m_registry->all_of<T>(m_entity_id);
		}

		template<typename T>
		void remove_component() const {
			if (!m_registry) {
				return;
			}

			if (!m_registry->valid(m_entity_id)) {
				return;
			}

			if (m_registry->all_of<T>(m_entity_id)) {
				m_registry->remove<T>(m_entity_id);
			}
		}

		inline entt::entity get_entt_id() const { return m_entity_id; }
		inline bool is_valid() const { return m_entity_id != entt::null; }

	private:
		entt::entity m_entity_id{ entt::null };
		// using a raw pointer because the registry is owned by the scene
		// only the scene should be able to delete the registry
		entt::registry* m_registry = nullptr;
	};
}
