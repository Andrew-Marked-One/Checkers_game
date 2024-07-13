#include "Vec2.h"


Vec2::Vec2() {}

bool Vec2::operator == (Vec2 rhs) const {
	float differenceX = std::abs(x - rhs.x);
	float differenceY = std::abs(y - rhs.y);
	return differenceX < 1e-6f && differenceY < 1e-6f;
}

bool Vec2::operator != (Vec2 rhs) const {
	float differenceX = std::abs(x - rhs.x);
	float differenceY = std::abs(y - rhs.y);
	return !(differenceX < 1e-6f && differenceY < 1e-6f);
}

Vec2 Vec2::operator + (Vec2 rhs) const {
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (Vec2 rhs) const {
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (Vec2 rhs) const {
	return Vec2(x / rhs.x, y / rhs.y);
}

Vec2 Vec2::operator * (Vec2 rhs) const {
	return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator + (float rhs) const {
	return Vec2(x + rhs, y + rhs);
}

Vec2 Vec2::operator - (float rhs) const {
	return Vec2(x - rhs, y - rhs);
}

Vec2 Vec2::operator / (float val) const {
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (float val) const {
	return Vec2(x * val, y * val);
}

void Vec2::operator += (float rhs) {
	x += rhs;
	y += rhs;
}

void Vec2::operator -= (float rhs) {
	x -= rhs;
	y -= rhs;
}

void Vec2::operator *= (float rhs) {
	x *= rhs;
	y *= rhs;
}

void Vec2::operator /= (float rhs) {
	x /= rhs;
	y /= rhs;
}

void Vec2::operator += (Vec2 rhs) {
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (Vec2 rhs) {
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (Vec2 rhs) {
	x *= rhs.x;
	y *= rhs.y;
}

void Vec2::operator /= (Vec2 rhs) {
	x /= rhs.x;
	y /= rhs.y;
}

float Vec2::distMag() const {
	return sqrt(x * x + y * y);
}

Vec2& Vec2::normalize() {
	float d = this->distMag();
	x /= d;
	y /= d;
	return *this;
}

Vec2& Vec2::round() {
	x = std::round(x);
	y = std::round(y);
	return *this;
}

Vec2& Vec2::abs() {
	x = std::abs(x);
	y = std::abs(y);
	return *this;
}

float Vec2::min() const {
	return std::min(x, y);
}

float Vec2::max() const {
	return std::max(x, y);
}

std::string Vec2::getStr() const {
	return std::to_string(x) + " " + std::to_string(y);
}