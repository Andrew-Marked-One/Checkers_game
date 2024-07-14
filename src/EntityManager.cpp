#include "EntityManager.h"


void EntityManager::update() {
	addEntities();
	removeEntities();
}

void EntityManager::addEntities() {
	for (auto& e : m_entitiesToAdd) {
		if (!e->hasTag("entity")) {
			e->addTag("entity");
		}

		for (auto& tag : e->tags()) {
			m_entityMap[tag].emplace_back(e.get());
		}

		m_entities.emplace_back(std::move(e));
	}
	m_entitiesToAdd.clear();
}

void EntityManager::removeEntities() {
	auto needErase = [this](const std::unique_ptr<Entity>& entity) {
		if (!entity->isAlive()) {
			removeFromEntityMap(entity.get());
			return true;
		}
		else {
			return false;
		}
	};

	auto eraseVec = std::ranges::remove_if(m_entities, needErase);
	m_entities.erase(eraseVec.begin(), eraseVec.end());
}

void EntityManager::removeFromEntityMap(const Entity* entity) {
	for (auto& tag : entity->tags()) {
		auto& eVec = m_entityMap[tag];
		auto it = std::ranges::lower_bound(eVec, entity->id(), std::less<>(), [](Entity* a) {
			return a->id();
		});
		eVec.erase(it);
	}
}

void EntityManager::addTag(Entity* entity, const std::string& tag) {
	if (!entity->hasTag(tag)) {
		entity->addTag(tag);

		auto& eVec = m_entityMap[tag];
		if (!eVec.empty()) {
			auto it = std::ranges::lower_bound(eVec, entity->id(), std::less<>(), [](Entity* a) {
				return a->id();
			});
			eVec.insert(it, entity);
		}
		else {
			eVec.emplace_back(entity);
		}
	}
}

std::vector<Entity*>& EntityManager::getEntities() {
	return m_entityMap["entity"];
}

const std::vector<Entity*>& EntityManager::getEntities() const {
	return m_entityMap.at("entity");
}

std::vector<Entity*>& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}

const std::vector<Entity*>& EntityManager::getEntities(const std::string& tag) const {
	return m_entityMap.at(tag);
}

Entity* EntityManager::getEntity(int id) {
	auto it = std::ranges::lower_bound(m_entities, id, std::less<>(), [](const std::unique_ptr<Entity>& a) {
		return a->id();
	});

	if (it != m_entities.end()) {
		return it->get();
	}
	else {
		std::cerr << "EntityManager::getEntity: no entity with id \"" << id << "\" found\n";
		return nullptr;
	}
}
