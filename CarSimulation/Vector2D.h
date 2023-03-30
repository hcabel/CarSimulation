#pragma once

#include <ostream>

struct IntVector2D;

struct Vector2D
{
	float x;
	float y;

	Vector2D();
	Vector2D(float x, float y);
	Vector2D(float value);
	Vector2D(const Vector2D& other);
	Vector2D(const IntVector2D& other);

	/* ADD */
	Vector2D operator+(const Vector2D& other) const;
	Vector2D operator+(const IntVector2D other) const;
	template<typename T2>
	Vector2D operator+(const T2 value) const;

	/* SUB */
	Vector2D operator-() const;
	Vector2D operator-(const Vector2D& other) const;
	Vector2D operator-(const IntVector2D other) const;
	template<typename T2>
	Vector2D operator-(const T2 value) const;

	/* MULT */
	Vector2D operator*(const Vector2D& other) const;
	template<typename T2>
	Vector2D operator*(const T2 value) const;

	/* DIV */
	Vector2D operator/(const Vector2D& other) const;
	template<typename T2>
	Vector2D operator/(const T2 value) const;

	/* EQUAL */
	Vector2D& operator=(const Vector2D& other);
	bool operator==(const Vector2D& other) const;
	Vector2D& operator+=(const Vector2D& other);
	Vector2D& operator-=(const Vector2D& other);
	Vector2D& operator*=(const Vector2D& other);
	Vector2D& operator/=(const Vector2D& other);
	bool operator!=(const Vector2D& other) const;

	Vector2D Round(const float precision = 0.0f) const;
	Vector2D Normalize() const;
	float Length() const;
	float Dot(const Vector2D& other) const;
	float AngleBetween(const Vector2D& other) const;

	Vector2D& Rotate(const float angle);

	static Vector2D Zero;

};

std::ostream& operator<<(std::ostream& os, const Vector2D& vector);
