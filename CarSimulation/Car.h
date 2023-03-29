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
	Car(const ATrack& track, uint8_t id, Vector2D spawnPoint, float acceleration = -1, float maxSpeed = -1)
		: m_Track(track),
		m_Id(id),
		m_Position(spawnPoint),
		// TODO: fix the random to be more evenly random (using std::max will just clamp the low value which make getting the lowest value more likely)
		m_MaxSpeed(CLAMP(CAR_MIN_ACCELERATION, 1.0f, maxSpeed == -1 ? static_cast<float>(std::rand()) / RAND_MAX : maxSpeed)),
		m_Acceleration(CLAMP(CAR_MIN_MAXSPEED, 1.0f, acceleration == -1 ? static_cast<float>(std::rand()) / RAND_MAX : acceleration))
	{
		std::cout << "Car " << GetDisplayChar() << " spawned at " << m_Position
			<< " maxspeed: " << m_MaxSpeed << " acceleration: " << m_Acceleration
			<< std::endl;
		IntVector2D currentTrackTilePosition = m_Track.MapPositionOnTrack(m_Position);
		char currentTrackTileDirectionChar = m_Track.GetTrackChar(currentTrackTilePosition);

		m_ForwardVector = GetDirectionVector(currentTrackTileDirectionChar);
		m_LastTrackDirection = currentTrackTileDirectionChar;
	}
	Car(const Car& other)
		: m_Track(other.m_Track),
		m_Id(other.m_Id),
		m_Position(other.m_Position),
		m_MaxSpeed(other.m_MaxSpeed),
		m_Acceleration(other.m_Acceleration),
		m_Speed(other.m_Speed),
		m_ForwardVector(other.m_ForwardVector),
		m_LastTrackDirection(other.m_LastTrackDirection)
	{}

	Car& operator=(const Car& other)
	{
		m_Position = other.m_Position;
		m_Speed = other.m_Speed;
		m_ForwardVector = other.m_ForwardVector;
		m_LastTrackDirection = other.m_LastTrackDirection;
		return *this;
	}

public:
	/** Move the car 1 step forward */
	void Move()
	{
		IntVector2D currentTrackTilePosition = m_Track.MapPositionOnTrack(m_Position);
		char currentTrackTileDirectionChar = m_Track.GetTrackChar(currentTrackTilePosition);

		// Accelerate or stop the car if there is an intersection ahead
		// TODO: implement deceleration instead of instant stop
		if (IsNextTileAnIntersection(currentTrackTilePosition, GetDirectionVector(m_LastTrackDirection))
			&& currentTrackTileDirectionChar != INTERSECTION
			&& IsNextIntersectionRedLightForMe(m_LastTrackDirection) == false)
		{
			m_Speed = 0.0f;
			return;
		}

		// Accelerate
		float newSpeed = std::min(m_MaxSpeed, m_Speed + m_Acceleration * m_MaxSpeed);
		// Get target point, where do we want to go next
		Vector2D newForwardVector = FindNextDirection(currentTrackTilePosition);

#if TRAFFIC_JAM_MODE
		// Look for a position that does not collide with any other car
		Vector2D newPosition;
		bool collideWithOtherCar;
		do
		{
			// Calcul a new position to test
			newPosition = m_Position + newForwardVector * Vector2D(newSpeed);

			// Check if the new position collide with any of the ca
			collideWithOtherCar = false;
			for (auto carPtr: m_Track.GetCarsOnTrack())
			{
				auto car = carPtr.lock();

				// Do not check collision with himself
				if (car->GetId() == m_Id)
					continue;

				float extraDistance = WillCollide(*car, newPosition);
				if (extraDistance < 0.0f)
				{
					// + because extra distance already negative ;)
					newSpeed = std::max(0.0f, newSpeed + extraDistance);
					collideWithOtherCar = true;
					break;
				}
			}
		} while (collideWithOtherCar && newSpeed > 0.0f);

		if (newSpeed <= 0.0f)
			m_Speed = 0.0f;
		else
		{
			m_Position = newPosition;
			m_ForwardVector = newForwardVector;
			m_Speed = newSpeed;
		}
#else
		m_Position = m_Position + newForwardVector * Vector2D(newSpeed);
		m_ForwardVector = newForwardVector;
		m_Speed = newSpeed;
#endif

		// Update directionChar
		m_LastTrackDirection = GetDirectionChar();
	}

	/**
	 * Check whether or not a point is colliding with the car.
	 *
	 * \param position Position of the point to check.
	 * \return true if the point is colliding with the car, false otherwise.
	 */
	bool Collide(Vector2D position) const
	{
		return (m_Position - position).Length() < CAR_SIZE_RADIUS;
	}
	float WillCollide(const Car& car, const Vector2D& fromThisPosition)
	{
		Vector2D vectorBetween = car.GetPosition() - fromThisPosition;
		float carsMininumDistanceRequired = CAR_SIZE_RADIUS * 2.0f;
		// Here there is a bug :D
		// when 2 cars follow each other too much the gab bewteen the vector length and the carsMinimumDistanceRequired is too small and the floating point bug
		// Example that I saw vector length equal 0.49999998 - 0.5000000 then the result became -2.21546894616e-8
		// TODO: Fix that if possible even though rounding is working pretty good
		float roundedVectorBetweenLengthFloat = std::round(vectorBetween.Length() * 1000.0f) / 1000.0f;
		return (roundedVectorBetweenLengthFloat - carsMininumDistanceRequired);
	}

	/**
	 * Simulate the state of the car after it move for N step forward.
	 *
	 * \param n the number of step forward you want to simulate.
	 * \return The state of the car after those n step.
	 */
	Car SimulateNMoveStep(uint32_t n) const
	{
		Car car = Car(*this);
		for (uint32_t i = 0; i < n; ++i)
			// car = car.CalculateNextPosition(car, car.GetTrack());
		return car;
	}

private:

	bool IsNextTileAnIntersection(const IntVector2D& currentTrackTilePosition, const IntVector2D& trackTileDirectionVector) const
	{
		return (m_Track.GetTrackChar(currentTrackTilePosition + trackTileDirectionVector) == INTERSECTION);
	}

	/**
	 * Return true or false based on whether or not the traffic light is red for you base on where you should go.
	 * note that using this all the intersection on the map are sync (and also only work on diagonal)
	 * TODO: make a traffic light class that handle itself.
	 *
	 * \param trackDirectionChar the direction char that tell you where to go.
	 * \return true if the intersection is red (and you have to stop) false if it's green (and you can continue)
	 */
	bool IsNextIntersectionRedLightForMe(char trackDirectionChar) const
	{
		constexpr int LightSwitchIntervalInSecond = 5;
		constexpr uint64_t NumberOfSecondInAMinute = 60ui64;

		// Calculate at which second were at in the current minute
		uint64_t amountOfSecondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int amountOfSecondsElapseInCurrentMinute = static_cast<int>(amountOfSecondsSinceEpoch % NumberOfSecondInAMinute);

		int moduloIndex = GetTraficLightModuloIndex(trackDirectionChar);
		if ((amountOfSecondsElapseInCurrentMinute / LightSwitchIntervalInSecond) % 4 == moduloIndex)
			return true;
		return false;
	}

	/** This function should be in a traffic light class */
	int GetTraficLightModuloIndex(const char trackDirectionChar) const
	{
		if (trackDirectionChar == RIGHT_DOWN || trackDirectionChar == LEFT_UP)
			return 0;
		else if (trackDirectionChar == UP_RIGHT || trackDirectionChar == DOWN_LEFT)
			return 1;
		else if (trackDirectionChar == RIGHT || trackDirectionChar == LEFT)
			return 2;
		else if (trackDirectionChar == UP || trackDirectionChar == DOWN)
			return 3;

		assert(false);
	}

	/**
	 * Get the direction (as a unit vector) the car should follow.
	 * We find this direction based on the track direction and by trying to stay in the middle of the road.
	 */
	Vector2D FindNextDirection(const IntVector2D& currentTrackTilePosition) const
	{

		// Find the point(target) that we want to go to
		// We do so by following the target point of our current track tile
		// and if the target point does not fit our requirement we check the next tile, and so on
		Vector2D targetPointDirection;
		Vector2D targetPointPosition;
		int stepForward = 0;
		do
		{
			if (stepForward >= 100)
				assert(false);

			// Move through the track 1 tile at the time to find the n ieme tile (where n = stepForward)
			targetPointPosition = currentTrackTilePosition;
			char targetPointDirectionChar = GetDirectionChar();
			for (int i = 0; i < stepForward; i++)
			{
				targetPointPosition += GetDirectionVector(targetPointDirectionChar);
				targetPointDirectionChar = m_Track.GetTrackChar(targetPointPosition);
				if (targetPointDirectionChar == CENTER)
					return (Vector2D(0.0f, 0.0f));
			}
			// Calculate the target point
			// here are the target point of a tile:
			// X-X-X
			// |   |
			// x   x
			// |   |
			// x-x-x
			// To calculate the target point we find the tile on track (15, 15)
			// we ad .5 to x and y to center our vector in the middle (15.5, 15.5)
			// then we offset the vector by the direction multiplied by 0.5
			// if the direction is (1, -1) our vector will be equal to (16, 15) which correspond to the UP_RIGHT target point
			Vector2D targetPointOffset = GetDirectionVector(targetPointDirectionChar) * Vector2D(0.5f, 0.5f);
			targetPointPosition = targetPointPosition + Vector2D(0.5f, 0.5f) + targetPointOffset;
			targetPointDirection = targetPointPosition - m_Position;
			stepForward += 1;
			// Search for next target point if the current target point is:
			// - too close (less than half of the distance that we will move in one step)
			// - The angle is to wide (do not allow 180 instant turn it's a simulation... crrappy... but a simulation ^^)
		} while (targetPointDirection.AngleBetween(m_ForwardVector) >= CAR_MAX_STEERINGANGLE_DEGREE || targetPointDirection.Length() < m_Speed / 2.0f);

		return (targetPointDirection.Normalize());
	}

public:
	const ATrack& GetTrack() const { return (m_Track); }

	Vector2D GetPosition() const { return (m_Position); }
	Vector2D GetForwardVector() const { return (m_ForwardVector); }
	float GetSpeed() const { return (m_Speed); }
	char GetLastTrackDirection() const { return (m_LastTrackDirection); }
	uint8_t GetId() const { return (m_Id); }
	char GetDisplayChar() const { return (static_cast<char>(m_Id + static_cast<uint8_t>('0'))); }

	char GetDirectionChar() const
	{
		auto trackPosition = m_Track.MapPositionOnTrack(m_Position);
		char trackChar = m_Track.GetTrackChar(trackPosition);
		if (trackChar == INTERSECTION)
			return (m_LastTrackDirection);
		return (trackChar);
	}

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