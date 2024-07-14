#pragma once
#include <cmath>
#include "SFML/Graphics.hpp"


class Vec2 {
public:
	float x = 0;
	float y = 0;

	Vec2();

	template <typename T>
	Vec2(T vec)
		: x(static_cast<float>(vec.x)), y(static_cast<float>(vec.y)) {}

	template <typename T1, typename T2>
	Vec2(T1 x_, T2 y_)
		: x(static_cast<float>(x_)), y(static_cast<float>(y_)) {}

	bool operator == (Vec2 rhs) const;
	bool operator != (Vec2 rhs) const;

	Vec2 operator + (Vec2 rhs) const;
	Vec2 operator - (Vec2 rhs) const;
	Vec2 operator / (Vec2 rhs) const;
	Vec2 operator * (Vec2 rhs) const;

	Vec2 operator + (float rhs) const;
	Vec2 operator - (float rhs) const;
	Vec2 operator / (float val) const;
	Vec2 operator * (float val) const;

	void operator += (float rhs);
	void operator -= (float rhs);
	void operator *= (float rhs);
	void operator /= (float rhs);

	void operator += (Vec2 rhs);
	void operator -= (Vec2 rhs);
	void operator *= (Vec2 rhs);
	void operator /= (Vec2 rhs);

	float distMag() const;
	Vec2& normalize();
	Vec2& round();
	Vec2& abs();
	float min() const;
	float max() const;
	std::string getStr() const;
};
