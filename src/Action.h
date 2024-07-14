#pragma once
#include "Vec2.h"


class Action {
public:
	Action();
	Action(const std::string& name, const std::string& type);
	Action(const std::string& name, const std::string& type, Vec2 pos);

	const std::string& name() const;
	const std::string& type() const;
	Vec2 pos() const;

private:
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	Vec2 m_pos = { 0, 0 };
};
	
