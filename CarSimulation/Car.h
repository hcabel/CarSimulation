#pragma once

#include "Vector2D.h"
#include "IntVector2D.h"
#include "Track.h"

#include <iostream>
#include <chrono>
#include <cassert>

#define CAR_SIZE_RADIUS 0.25f
#define CAR_MIN_ACCELERATION 0.1f
#define CAR_MIN_MAXSPEED 0.5f
#define CAR_MAX_STEERINGANGLE_DEGREE 45.0f

/**
 * Car that will ride onto the track.
 */
class Car
{

public:
	Car(const ATrack& track, uint8_t id, Vector2D spawnPoint, float acceleration = -1, float maxSpeed = -1);
	Car(const Car& other);
	Car& operator=(const Car& other);

public:
	/** Move the car 1 step forward */
	void Move();

	/**
	 * Check whether or not a point is colliding with the car.
	 *
	 * \param position Position of the point to check.
	 * \return true if the point is colliding with the car, false otherwise.
	 */
	bool IsColliding(Vector2D position) const { return (m_Position - position).Length() < CAR_SIZE_RADIUS; }
	/**
	 * Calculate the extra distance between two car.
	 * Extra mean the distance between the two cars minus the radius of both cars.
	 *
	 * \param car The other car that we want to calculate the extra distance with.
	 * \param fromThisPosition The position that we want to calculate the extra distance from.
	 * \return The extra distance between the two cars.
	 */
	float FindExtraDistanceBetweenCars(const std::shared_ptr<Car>& car, const Vector2D& fromThisPosition) const;

private:

	Vector2D FindNextLaneDirection(const IntVector2D& currentTrackTilePosition, char currentTrackTileDirectionChar) const;

	/**
	 * Calculate the optimum speed without crashing in any other car.
	 *
	 * \param currentSpeed The current speed of the car.
	 * \param direction The direction of the car.
	 * \return The maximum speed without crashing in any other car.
	 */
	float CalculateMaxSpeedWithoutCollision(float currentSpeed, const Vector2D& direction) const;

	/**
	 * Check if the car is colliding with any other car at a give position.
	 *
	 * \param position the position to check.
	 * \param cars the list of cars to check collision with.
	 * \param outExtraDistance the extra distance to move to not collide with any other car. (positive)
	 * \return true if the car is colliding with any other car, false otherwise.
	 */
	bool IsCollidingWithOtherCar(const Vector2D& position, const std::vector<std::weak_ptr<Car>>& cars, float* outExtraDistance = nullptr) const;

	bool IsNextTileAnIntersection(const IntVector2D& currentTrackTilePosition, const IntVector2D& trackTileDirectionVector) const;

	/**
	 * Return true or false based on whether or not the traffic light is red for you base on where you should go.
	 * note that using this all the intersection on the map are sync (and also only work on diagonal)
	 * TODO: make a traffic light class that handle itself.
	 *
	 * \param trackDirectionChar the direction char that tell you where to go.
	 * \return true if the intersection is red (and you have to stop) false if it's green (and you can continue)
	 */
	bool IsNextIntersectionRedLightForMe(char trackDirectionChar) const;

	/** This function should be in a traffic light class */
	int GetTraficLightModuloIndex(const char trackDirectionChar) const;

	/**
	 * Get the direction (as a unit vector) the car should follow.
	 * We find this direction based on the track direction and by trying to stay in the middle of the road.
	 */
	Vector2D FindNextDirection(const IntVector2D& currentTrackTilePosition) const;

public:
	const ATrack& GetTrack() const { return (m_Track); }

	Vector2D GetPosition() const { return (m_Position); }
	Vector2D GetForwardVector() const { return (m_ForwardVector); }
	float GetSpeed() const { return (m_Speed); }
	char GetLastTrackDirection() const { return (m_LastTrackDirection); }
	uint8_t GetId() const { return (m_Id); }
	char GetDisplayChar() const { return (static_cast<char>(m_Id + static_cast<uint8_t>('0'))); }

	char GetDirectionChar() const;

private:
	/** Reference onto the track that the cars is currently driving onto */
	const ATrack& m_Track;
	/** Id of the car */
	const uint8_t m_Id;
	/* Car max speed, (between 0 -> 1) */
	const float m_MaxSpeed;
	/* Car acceleration relative to max speed (.1 acc equal to + .05 speed if maxspeed = 0.5) */
	const float m_Acceleration;

	/** Position of the car */
	Vector2D m_Position;
	/** unit vector representing where the car is heading */
	Vector2D m_ForwardVector;
	/* Car current speed (between 0 -> 1) */
	float m_Speed = 0.0f;
	/** The last track direction char that the car has follow */
	char m_LastTrackDirection;

};
