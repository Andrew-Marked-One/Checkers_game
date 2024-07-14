#pragma once
#include "Entity.h"


class EntityManager {
public:
	template <typename T>
		requires isStringLike<T>
	Entity* addEntity(T&& tag) {
		return addEntityHelper(std::vector<std::string>{ std::forward<T>(tag) });
	}

	template <typename T>
		requires isStringVec<T>
	Entity* addEntity(T&& tags) {
		return addEntityHelper(std::forward<T>(tags));
	}

	void update();
	void addTag(Entity* entity, const std::string& tag);
	std::vector<Entity*>& getEntities();
	const std::vector<Entity*>& getEntities() const;
	std::vector<Entity*>& getEntities(const std::string& tag);
	const std::vector<Entity*>& getEntities(const std::string& tag) const;
	Entity* getEntity(int id);

private:
	std::unordered_map<std::string, std::vector<Entity*>> m_entityMap;
	std::vector<std::unique_ptr<Entity>> m_entities;
	std::vector<std::unique_ptr<Entity>> m_entitiesToAdd;
	size_t m_totalEntities = 0;

	template <typename T>
		requires isStringVec<T>
	Entity* addEntityHelper(T&& tags) {
		auto& ptr = m_entitiesToAdd.emplace_back(std::make_unique<Entity>(m_totalEntities++, std::forward<T>(tags)));
		return ptr.get();
	}

	void addEntities();
	void removeEntities();
	void removeFromEntityMap(const Entity* entity);
};
