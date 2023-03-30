#pragma once
#pragma once

#include <ostream>

struct Vector2D;

struct IntVector2D
{
	int x;
	int y;

	IntVector2D();
	IntVector2D(int x, int y);
	IntVector2D(int value);
	IntVector2D(const IntVector2D& other);
	IntVector2D(const Vector2D& other);

	/* ADD */
	IntVector2D operator+(const IntVector2D& other) const;
	Vector2D operator+(const Vector2D other) const;
	template<typename T2>
	IntVector2D operator+(const T2 value) const;

	/* SUB */
	IntVector2D operator-() const;
	IntVector2D operator-(const IntVector2D& other) const;
	Vector2D operator-(const Vector2D other) const;
	template<typename T2>
	IntVector2D operator-(const T2 value) const;

	/* MULT */
	IntVector2D operator*(const IntVector2D& other) const;
	Vector2D operator*(const Vector2D& other) const;
	template<typename T2>
	IntVector2D operator*(const T2 value) const;
	/* DIV */
	IntVector2D operator/(const IntVector2D& other) const;
	template<typename T2>
	IntVector2D operator/(const T2 value) const;

	/* EQUAL */
	IntVector2D& operator=(const IntVector2D& other);
	bool operator==(const IntVector2D& other) const;
	IntVector2D& operator+=(const IntVector2D& other);
	IntVector2D& operator-=(const IntVector2D& other);
	IntVector2D& operator*=(const IntVector2D& other);
	IntVector2D& operator/=(const IntVector2D& other);
	bool operator!=(const IntVector2D& other) const;

	Vector2D Normalize() const;
	float Length() const;
	float Dot(const IntVector2D& other) const;
	float Angle(const IntVector2D& other) const;
};

std::ostream& operator<<(std::ostream& os, const IntVector2D& vector);
