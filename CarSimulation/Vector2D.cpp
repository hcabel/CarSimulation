#include "Vector2D.h"
#include "IntVector2D.h"

#include <cmath>

Vector2D::Vector2D()
	: x(0.0f), y(0.0f)
{}

Vector2D::Vector2D(float x, float y)
	: x(x), y(y)
{}

Vector2D::Vector2D(float value)
	: x(value), y(value)
{}

Vector2D::Vector2D(const Vector2D& other)
	: x(other.x), y(other.y)
{}

Vector2D::Vector2D(const IntVector2D& other)
	: x(static_cast<float>(other.x)), y(static_cast<float>(other.y))
{}

Vector2D Vector2D::operator+(const Vector2D& other) const
{
	return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator+(const IntVector2D other) const
{
	return Vector2D(x + other.x, y + other.y);
}

Vector2D Vector2D::operator-() const
{
	return Vector2D(-x, -y);
}

Vector2D Vector2D::operator-(const Vector2D& other) const
{
	return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator-(const IntVector2D other) const
{
	return Vector2D(x - other.x, y - other.y);
}

Vector2D Vector2D::operator*(const Vector2D& other) const
{
	return Vector2D(x * other.x, y * other.y);
}

Vector2D Vector2D::operator/(const Vector2D& other) const
{
	return Vector2D(x / other.x, y / other.y);
}

Vector2D& Vector2D::operator=(const Vector2D& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

bool Vector2D::operator==(const Vector2D& other) const
{
	return x == other.x && y == other.y;
}

Vector2D& Vector2D::operator+=(const Vector2D& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2D& Vector2D::operator*=(const Vector2D& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

Vector2D& Vector2D::operator/=(const Vector2D& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}

bool Vector2D::operator!=(const Vector2D& other) const
{
	return false;
}

Vector2D Vector2D::Round(const float precision) const
{
	if (precision == 0.0f)
		return Vector2D(std::round(x), std::round(y));
	return Vector2D(std::round(x / precision) * precision, std::round(y / precision) * precision);
}

Vector2D Vector2D::Normalize() const
{
	float length = Length();
	if (length == 0.0f)
		return Vector2D(0.0f, 0.0f);
	return Vector2D(x / length, y / length);
}

float Vector2D::Length() const
{
	return std::sqrt(x * x + y * y);
}

float Vector2D::Dot(const Vector2D& other) const
{
	return (x * other.x) + (y * other.y);
}

float Vector2D::AngleBetween(const Vector2D& other) const
{
	return (std::acos(Dot(other) / (Length() * other.Length())));
}

template<typename T2>
Vector2D Vector2D::operator+(const T2 value) const
{
	return Vector2D(x + value, y + value);
}

template<typename T2>
Vector2D Vector2D::operator-(const T2 value) const
{
	return Vector2D(x - value, y - value);
}

template<typename T2>
Vector2D Vector2D::operator*(const T2 value) const
{
	return Vector2D(x * value, y * value);
}

template<typename T2>
Vector2D Vector2D::operator/(const T2 value) const
{
	return Vector2D(x / value, y / value);
}

std::ostream& operator<<(std::ostream& os, const Vector2D& vector)
{
	os << "(x" << vector.x << ", y" << vector.y << ")";
	return (os);
}