#pragma once

#include "Vector2D.h"
#include "IntVector2D.h"
#include "Track.h"

#include <iostream>
#include <chrono>

/**
 * Car that will ride onto the track.
 */
class Car
{

public:
	Car(const FigureEightTrack& track, uint8_t number, Vector2D spawnPoint, float acceleration = -1, float maxSpeed = -1)
		: m_Track(track),
		m_Number(number),
		m_Position(spawnPoint),
		// TODO: fix the random to be more evenly random (using std::max will just clamp the low value which make getting the lowest value more likely)
		m_MaxSpeed(std::max(MinimumCarsAcceleration, maxSpeed == -1 ? static_cast<float>(std::rand()) / RAND_MAX : maxSpeed)),
		m_Acceleration(std::max(MinimumCarsMaxSpeed, acceleration == -1 ? static_cast<float>(std::rand()) / RAND_MAX : acceleration))
	{
		std::cout << "Car " << GetDisplayNumber() << " spawned at " << m_Position
			<< " maxspeed: " << m_MaxSpeed << " acceleration: " << m_Acceleration
			<< std::endl;
	}
	Car(const Car& other)
		: m_Track(other.m_Track),
		m_Number(other.m_Number),
		m_Position(other.m_Position),
		m_MaxSpeed(other.m_MaxSpeed),
		m_Acceleration(other.m_Acceleration),
		m_Speed(other.m_Speed),
		m_ForwardVector(other.m_ForwardVector),
		m_LastTrackDirection(other.m_LastTrackDirection)
	{
		std::cout << "Car has been cloned\n";
	}

	Car& operator=(const Car& other)
	{
		SetPosition(other.GetPosition());
		SetSpeed(other.GetSpeed());
		SetForwardVector(other.GetForwardVector());
		SetLastTrackDirection(other.GetLastTrackDirection());
		return *this;
	}

public:
	/** Move the car 1 step forward */
	void Move()
	{
		Car nextStep = CalculateNextPosition(*this, m_Track);
		SetPosition(nextStep.GetPosition());
		SetSpeed(nextStep.GetSpeed());
		SetForwardVector(nextStep.GetForwardVector());
		SetLastTrackDirection(nextStep.GetLastTrackDirection());
	}

	/**
	 * Check whether or not a point is colliding with the car.
	 *
	 * \param position Position of the point to check.
	 * \return true if the point is colliding with the car, false otherwise.
	 */
	bool Collide(Vector2D position) const
	{
		float carSize = 0.25f;
		return (m_Position - position).Length() < carSize;
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
			car = car.CalculateNextPosition(car, car.GetTrack());
		return car;
	}

private:
	/**
	 * Perform all the calculations to find the next position of the car.
	 *
	 * \param carBeginState The state of the car.
	 * \return The next state of the car.
	 */
	Car CalculateNextPosition(const Car& carBeginState, const FigureEightTrack& track) const
	{
		Car car = Car(carBeginState);

		IntVector2D currentTrackTilePosition = track.MapOntoTrack(car.GetPosition());
		char currentTrackTileDirectionChar = track.GetTrackChar(currentTrackTilePosition);
		IntVector2D currentTrackTileDirectionVector = track.GetTrackDirection(currentTrackTileDirectionChar);

		// Accelerate or stop the car if there is an intersection ahead
		// TODO: implement deceleration instead of instant stop
		if (IsNextTileAnIntersection(track, currentTrackTilePosition, currentTrackTileDirectionVector)
			&& NextIntersectionIsRedLightForMe(currentTrackTileDirectionChar != INTERSECTION ? currentTrackTileDirectionChar : car.GetLastTrackDirection()))
		{
			car.SetSpeed(0.0f);
			return (car);
		}

		// Accelerate
		car.SetSpeed(std::min(car.m_MaxSpeed, car.m_Speed + car.m_Acceleration * car.m_MaxSpeed));

		// Get target point, where do we want to go next
		car.SetForwardVector(FindNextDirection(track, car, currentTrackTilePosition));

		char trackChar = track.GetTrackChar(m_Track.MapOntoTrack(carBeginState.m_Position));
		return (car);
	}

	bool IsNextTileAnIntersection(const FigureEightTrack& track, const IntVector2D& currentTrackTilePosition, const IntVector2D& trackTileDirectionVector) const
	{
		return (track.GetTrackChar(currentTrackTilePosition + trackTileDirectionVector) == INTERSECTION);
	}

	/**
	 * Return true or false based on whether or not the traffic light is red for you base on where you should go.
	 * note that using this all the intersection on the map are sync (and also only work on diagonal)
	 * TODO: make a traffic light class that handle itself.
	 *
	 * \param trackDirectionChar the direction char that tell you where to go.
	 * \return
	 */
	bool NextIntersectionIsRedLightForMe(char trackDirectionChar) const
	{
		constexpr int LightSwitchIntervalInSecond = 5;
		constexpr uint64_t NumberOfSecondInAMinute = 60ui64;

		// Calculate at which second were at in the current minute
		uint64_t amountOfSecondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		int amountOfSecondsElapseInCurrentMinute = static_cast<int>(amountOfSecondsSinceEpoch % NumberOfSecondInAMinute);

		// If it's were moving right and down, then the light is Green if the number of second in the current minute divided by the light switch interval is even
		int evenOrOdd = (trackDirectionChar == RIGHT_DOWN ? 0 : 1);

		// stop the car if the light is off (TODO: implement deceleration instead of instant stop)
		if ((amountOfSecondsElapseInCurrentMinute / LightSwitchIntervalInSecond) % 2 == evenOrOdd)
			return true;
		return false;
	}

	Vector2D FindNextDirection(const FigureEightTrack& track, const Car& car, const IntVector2D& currentTrackTilePosition) const
	{
		constexpr float MaxSteeringAngle = 45.0f;
		constexpr float MinDistanceBetweenCarAndTargetPoint = 0.5f;

		// Find target point and his direction
		Vector2D targetPointDirection;
		Vector2D targetPointPosition = car.GetPosition();
		float stepForward = 0;
		float angle;
		do
		{
			targetPointPosition = currentTrackTilePosition;
			char targetTrackChar = car.GetLastTrackDirection();
			for (int i = 0; i < stepForward; i++)
			{
				targetPointPosition += GetDirectionVector(targetTrackChar);
				targetTrackChar = track.GetTrackChar(targetPointPosition);
			}
			targetPointDirection = GetDirectionVector(targetTrackChar);
			stepForward += 1;
			angle = targetPointDirection.AngleBetween(car.GetForwardVector());
		} while (angle >= MaxSteeringAngle && (targetPointPosition - car.GetPosition()).Length() < MinDistanceBetweenCarAndTargetPoint);

		return (targetPointDirection);
	}

public:
	const FigureEightTrack& GetTrack() const { return (m_Track); }

	Vector2D GetPosition() const { return (m_Position); }
	Vector2D GetForwardVector() const { return (m_ForwardVector); }
	float GetSpeed() const { return (m_Speed); }
	char GetLastTrackDirection() const { return (m_LastTrackDirection); }
	uint8_t GetNumber() const { return (m_Number); }
	char GetDisplayNumber() const { return (static_cast<char>(m_Number + static_cast<uint8_t>('0'))); }

private:
	void SetPosition(const Vector2D& position) { m_Position = position; }
	void SetForwardVector(const Vector2D& forwardVector) { m_ForwardVector = forwardVector; }
	void SetSpeed(float speed) { m_Speed = speed; }
	void SetLastTrackDirection(char lastTrackDirection) { m_LastTrackDirection = lastTrackDirection; }

private:
	/** Reference onto the track that the cars is currently driving onto */
	const FigureEightTrack& m_Track;
	/** Id of the car */
	const uint8_t m_Number;
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
