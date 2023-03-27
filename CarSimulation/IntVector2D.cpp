#include "IntVector2D.h"
#include "Vector2D.h"

#include <cmath>

IntVector2D::IntVector2D()
	: x(0), y(0)
{}

IntVector2D::IntVector2D(int x, int y)
	: x(x), y(y)
{}

IntVector2D::IntVector2D(int value)
	: x(value), y(value)
{}

IntVector2D::IntVector2D(const IntVector2D& other)
	: x(other.x), y(other.y)
{}

IntVector2D::IntVector2D(const Vector2D& other)
	: x(static_cast<int>(other.x)), y(static_cast<int>(other.y))
{}

IntVector2D IntVector2D::operator+(const IntVector2D& other) const
{
	return IntVector2D(x + other.x, y + other.y);
}

IntVector2D IntVector2D::operator+(const Vector2D other) const
{
	return IntVector2D(x + static_cast<int>(std::round(other.x)), y + static_cast<int>(std::round(other.y)));
}

IntVector2D IntVector2D::operator-() const
{
	return IntVector2D(-x, -y);
}

IntVector2D IntVector2D::operator-(const IntVector2D& other) const
{
	return IntVector2D(x - other.x, y - other.y);
}

IntVector2D IntVector2D::operator-(const Vector2D other) const
{
	return IntVector2D(x - static_cast<int>(std::round(other.x)), y - static_cast<int>(std::round(other.y)));
}

IntVector2D IntVector2D::operator*(const IntVector2D& other) const
{
	return IntVector2D(x * other.x, y * other.y);
}

Vector2D IntVector2D::operator*(const Vector2D& other) const
{
	return Vector2D(x * other.x, y * other.y);
}

IntVector2D IntVector2D::operator/(const IntVector2D& other) const
{
	return IntVector2D(x / other.x, y / other.y);
}

IntVector2D& IntVector2D::operator=(const IntVector2D& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

bool IntVector2D::operator==(const IntVector2D& other) const
{
	return x == other.x && y == other.y;
}

IntVector2D& IntVector2D::operator+=(const IntVector2D& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

IntVector2D& IntVector2D::operator-=(const IntVector2D& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

IntVector2D& IntVector2D::operator*=(const IntVector2D& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

IntVector2D& IntVector2D::operator/=(const IntVector2D& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}

bool IntVector2D::operator!=(const IntVector2D& other) const
{
	return false;
}

IntVector2D IntVector2D::Normalize() const
{
	float length = Length();
	if (length == 0.0f)
		return IntVector2D(0, 0);
	return IntVector2D(Vector2D(x / length, y / length).Round());
}

float IntVector2D::Length() const
{
	return static_cast<float>(std::sqrt(x * x + y * y));
}

float IntVector2D::Dot(const IntVector2D& other) const
{
	return (x * static_cast<float>(other.x) + y * static_cast<float>(other.y));
}

float IntVector2D::Angle(const IntVector2D& other) const
{
	return (std::acos(Dot(other) / (Length() * other.Length())));
}

template<typename T2>
IntVector2D IntVector2D::operator+(const T2 value) const
{
	return IntVector2D(x + value, y + value);
}

template<typename T2>
IntVector2D IntVector2D::operator-(const T2 value) const
{
	return IntVector2D(x - value, y - value);
}

template<typename T2>
IntVector2D IntVector2D::operator*(const T2 value) const
{
	return IntVector2D(x * value, y * value);
}

template<typename T2>
IntVector2D IntVector2D::operator/(const T2 value) const
{
	return IntVector2D(x / value, y / value);
}

std::ostream& operator<<(std::ostream& os, const IntVector2D& vector)
{
	os << "(x" << vector.x << ", y" << vector.y << ")";
	return (os);
}