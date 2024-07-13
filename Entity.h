#pragma once
#include <memory>
#include <tuple>
#include <ranges>
#include "Components.h"
#include "MyConcepts.h"


class EntityManager;

class Entity {
	using ComponentTuple = std::tuple<
		std::unique_ptr<CTransform>,
		std::unique_ptr<CAnimation>,
		std::unique_ptr<CShape>,
		std::unique_ptr<CStateCell>,
		std::unique_ptr<CStateUI>,
		std::unique_ptr<CText>,
		std::unique_ptr<CFunctions>
	>;

	friend class EntityManager;

public:
	int row = -1;
	int col = -1;

	template <typename T>
		requires isStringVec<T>
	Entity(int id, T&& tags)
		: m_id{ id }
		, m_tags{ std::forward<T>(tags) } {}


	int id() const;
	const std::vector<std::string>& tags() const;
	bool hasTag(const std::string& tag) const;
	bool isAlive() const;
	bool isHiding() const;
	void destroy();
	void hide();
	void reveal();
	
	template <typename T>
		requires isTypeInUniquePtrTuple<T, Entity::ComponentTuple>
	bool hasComponent() const {
		return getComponentPtr<T>() != nullptr;
	}

	template <typename T, typename... Args>
		requires isTypeInUniquePtrTuple<T, Entity::ComponentTuple>
	T& addComponent(Args&&... args) {
		auto& component = getComponentPtr<T>();
		component = std::make_unique<T>(std::forward<Args>(args)...);
		return *component;
	}

	template <typename T>
		requires isTypeInUniquePtrTuple<T, Entity::ComponentTuple>
	const T& getComponent() const {
		if (!hasComponent<T>()) {
			throwComponentNotFoundError<T>();
		}
		return *getComponentPtr<T>();
	}

	template <typename T>
		requires isTypeInUniquePtrTuple<T, Entity::ComponentTuple>
	T& getComponent() {
		if (!hasComponent<T>()) {
			throwComponentNotFoundError<T>();
		}
		return *getComponentPtr<T>();
	}

	template <typename T>
	void removeComponent() {
		getComponentPtr<T>().reset();
	}

private:
	ComponentTuple m_components;
	std::vector<std::string> m_tags = { "default" };
	int m_id = 0;
	bool m_isAlive = true;
	bool m_isHiding = false;

	template <typename T>
		requires isStringLike<T>
	void addTag(T&& tag) {
		m_tags.emplace_back(std::forward<T>(tag));
	}

	template <typename T>
	const std::unique_ptr<T>& getComponentPtr() const {
		return std::get<std::unique_ptr<T>>(m_components);
	}

	template <typename T>
	std::unique_ptr<T>& getComponentPtr() {
		return std::get<std::unique_ptr<T>>(m_components);
	}

	template <typename T>
	void throwComponentNotFoundError() const {
		std::cerr << "Entity::getComponent: entity with id \"" << m_id << "\" doesn't have \n" + std::string(typeid(T).name()) + "\n component\n";
		std::cerr << "entity tags: ";
		for (auto& str : m_tags) {
			std::cerr << str << ' ';
		}
		std::cerr << '\n';
		throw std::runtime_error("Component not found");
	}
};