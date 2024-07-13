#include "Entity.h"


int Entity::id() const {
	return m_id;
}

const std::vector<std::string>& Entity::tags() const {
	return m_tags;
}

bool Entity::hasTag(const std::string& tag) const {
	return std::ranges::find(m_tags, tag) != m_tags.end();
}

bool Entity::isAlive() const {
	return m_isAlive;
}

void Entity::destroy() {
	m_isAlive = false;
}

bool Entity::isHiding() const {
	return m_isHiding;
}

void Entity::hide() {
	m_isHiding = true;
}

void Entity::reveal() {
	m_isHiding = false;
}