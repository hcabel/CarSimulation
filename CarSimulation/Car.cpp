#include "Car.h"

Car::Car(const ATrack& track, uint8_t id, Vector2D spawnPoint, float acceleration, float maxSpeed)
	: m_Track(track),
	m_Id(id),
	m_Position(spawnPoint),
	// TODO: fix the random to be more evenly random (using std::max will just clamp the low value which make getting the lowest value more likely)
	m_MaxSpeed(CLAMP(CAR_MIN_MAXSPEED, CAR_MAX_MAXSPEED, maxSpeed == -1 ? static_cast<float>(std::rand()) / RAND_MAX : maxSpeed)),
	m_Acceleration(CLAMP(CAR_MIN_ACCELERATION, CAR_MAX_ACCELERATION, acceleration == -1 ? static_cast<float>(std::rand()) / RAND_MAX : acceleration))
{
	std::cout << "Car " << GetDisplayChar() << " spawned at " << m_Position
		<< " maxspeed: " << m_MaxSpeed << " acceleration: " << m_Acceleration
		<< std::endl;
	IntVector2D currentTrackTilePosition = m_Track.MapPositionOnTrack(m_Position);
	char currentTrackTileDirectionChar = m_Track.GetTrackChar(currentTrackTilePosition);

	m_ForwardVector = GetDirectionVector(currentTrackTileDirectionChar);
	m_LastTrackDirection = currentTrackTileDirectionChar;
}

Car::Car(const Car& other)
	: m_Track(other.m_Track),
	m_Id(other.m_Id),
	m_Position(other.m_Position),
	m_MaxSpeed(other.m_MaxSpeed),
	m_Acceleration(other.m_Acceleration),
	m_Speed(other.m_Speed),
	m_ForwardVector(other.m_ForwardVector),
	m_LastTrackDirection(other.m_LastTrackDirection)
{}

Car& Car::operator=(const Car& other)
{
	m_Position = other.m_Position;
	m_Speed = other.m_Speed;
	m_ForwardVector = other.m_ForwardVector;
	m_LastTrackDirection = other.m_LastTrackDirection;
	return *this;
}

void Car::Move()
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

	// Get target point, where do we want to go next (forward)
	Vector2D newDirection;
	if (currentTrackTileDirectionChar != CENTER)
		newDirection = FindNextDirection(currentTrackTilePosition);
	else
	{
		// In case something wrong happen we keep our current direction
		// but if were leaving the map we change the direction toward the center of the map
		if (m_Track.IsHereInMapBounds(currentTrackTilePosition))
			m_ForwardVector = m_Position - m_Track.GetMapCenter();
		newDirection = m_ForwardVector;
	}

#if DRIVING_MODE == 0 // no collision just follow the road
	m_Speed = newSpeed;
	m_ForwardVector = newDirection;
	m_Position = m_Position + newDirection * Vector2D(newSpeed);
#elif DRIVING_MODE == 1 // collision detection (traffic jam simulator)
	// Compute new position
	Vector2D positionToCheck = m_Position + newDirection * Vector2D(newSpeed + SAFE_DISTANCE_BETWEEN_CARS);

	float extraCollidingDistance;
	if (IsCollidingWithOtherCar(positionToCheck, m_Track.GetCarsOnTrack(), &extraCollidingDistance))
		newSpeed = CalculateMaxSpeedWithoutCollision(newSpeed - extraCollidingDistance, newDirection);

	// Move the car
	m_Position += newDirection * Vector2D(newSpeed);
	m_ForwardVector = newDirection;
	m_Speed = newSpeed;

#else // collision + lane change (Work In Progress)
	// Compute new position
	Vector2D positionToCheck = m_Position + newDirection * Vector2D(newSpeed + SAFE_DISTANCE_BETWEEN_CARS);

	float extraCollidingDistance;
	if (IsCollidingWithOtherCar(positionToCheck, m_Track.GetCarsOnTrack(), &extraCollidingDistance))
	{
		// If there is a car in front of you try to change lane
		Vector2D newLaneDirection = FindNextLaneDirection(currentTrackTilePosition, currentTrackTileDirectionChar);

		if (newLaneDirection != Vector2D::Zero)
		{
			// Compute position when changing lane
			positionToCheck = m_Position + newLaneDirection * Vector2D(newSpeed);

			// check if it collide with any of the cars
			if (IsCollidingWithOtherCar(positionToCheck, m_Track.GetCarsOnTrack()))
			{
				// Slow down to avoid crashing into the car in front of you
				newSpeed = CalculateMaxSpeedWithoutCollision(newSpeed - extraCollidingDistance, newDirection);
			}
			else if (m_Track.GetTrackChar(m_Track.MapPositionOnTrack(positionToCheck)) == CENTER)
			{
				// Slow down to avoid getting out of track
				m_Speed = (positionToCheck - m_Position).Length();
			}
			else
			{
				newDirection = newLaneDirection;
			}
		}
		else
			// Slow down to avoid crashing into the car in front of you
			newSpeed = CalculateMaxSpeedWithoutCollision(newSpeed - extraCollidingDistance, newDirection);
	}

	// Move the car
	m_Position += newDirection * Vector2D(newSpeed);
	m_ForwardVector = newDirection;
	m_Speed = newSpeed;
#endif

	// Update directionChar
	m_LastTrackDirection = GetDirectionChar();
}

bool Car::IsColliding(const std::shared_ptr<Car>& car) const
{
	Vector2D vectorBetween = car->GetPosition() - m_Position;
	float carsMininumDistanceRequired = CAR_SIZE_RADIUS * 2.0f;
	return (vectorBetween.Length() <= carsMininumDistanceRequired);
}

float Car::FindExtraDistanceBetweenCars(const std::shared_ptr<Car>& car, const Vector2D& fromThisPosition) const
{
	Vector2D vectorBetween = car->GetPosition() - fromThisPosition;
	float carsMininumDistanceRequired = CAR_SIZE_RADIUS * 2.0f;
	// Here there is a bug :D
	// when 2 cars follow each other too much the gab bewteen the vector length and the carsMinimumDistanceRequired is too small and the floating point bug
	// Example that I saw vector length equal 0.49999998 - 0.5000000 then the result became -2.21546894616e-8
	// TODO: Fix that if possible even though rounding is working pretty good
	float roundedVectorBetweenLengthFloat = std::round(vectorBetween.Length() * 10000.0f) / 10000.0f;
	return (roundedVectorBetweenLengthFloat - carsMininumDistanceRequired);
}

Vector2D Car::FindNextLaneDirection(const IntVector2D& currentTrackTilePosition, char currentTrackTileDirectionChar) const
{
	// Get road direction
	Vector2D newLaneDirection = GetDirectionVector(currentTrackTileDirectionChar).Normalize();
	if (newLaneDirection == Vector2D::Zero)
		return Vector2D::Zero;

	// Rotate the vector to 45 degree
	newLaneDirection = newLaneDirection.Rotate(45.0f);
	IntVector2D tilePosition = m_Track.MapPositionOnTrack(currentTrackTilePosition + newLaneDirection);
	// Check if it's a road, and if it is check if the road is going in the same direction
	// TODO: handle the case where the next lane direction is less than 45 degree from the current direction
	if (m_Track.IsHereARoad(tilePosition) == false
		|| m_Track.GetTrackChar(tilePosition) != currentTrackTileDirectionChar)
	{
		// Rotate the vector to -45 degree
		newLaneDirection = newLaneDirection.Rotate(-90.0f);
		tilePosition = m_Track.MapPositionOnTrack(currentTrackTilePosition + newLaneDirection);
		if (m_Track.IsHereARoad(tilePosition) == false
			|| m_Track.GetTrackChar(tilePosition) != currentTrackTileDirectionChar)
			return Vector2D::Zero;
	}

	return ((tilePosition + Vector2D(0.5f, 0.5f)) - m_Position).Normalize();
}

float Car::CalculateMaxSpeedWithoutCollision(float currentSpeed, const Vector2D& direction) const
{
	float bestSpeed = currentSpeed;
	float extraSpeed;
	bool isColliding;

	int i = 0;
	// using IsCollidingWithOtherCar we should be able to calculate the extra distance to move to not collide with any other car in one go
	// but just in case we verify that there is no collision
	do
	{
		// Weird bug with exponential floating point (describe 2 function before)
		i++;
		if (i >= 100)
		{
			bestSpeed = 0.0f;
			break;
		}
		isColliding = IsCollidingWithOtherCar(
			m_Position + direction * Vector2D(bestSpeed),
			m_Track.GetCarsOnTrack(), &extraSpeed);
		if (isColliding)
			bestSpeed -= extraSpeed;

	} while (isColliding && bestSpeed > 0.0f);

	return std::max(0.0f, bestSpeed - SAFE_DISTANCE_BETWEEN_CARS);
}

bool Car::IsCollidingWithOtherCar(const Vector2D& position, const std::vector<std::weak_ptr<Car>>& cars, float* outExtraDistance) const
{
	float closestCarDistance = std::numeric_limits<float>::max();
	std::shared_ptr<Car> closestCar;

	// Find the closestCar car
	for (auto carPtr: m_Track.GetCarsOnTrack())
	{
		auto car = carPtr.lock();

		// Do not check collision with himself
		if (car->GetId() == m_Id)
			continue;

		float distanceBetweenCars = FindExtraDistanceBetweenCars(car, position);
		if (distanceBetweenCars < closestCarDistance)
		{
			closestCarDistance = distanceBetweenCars;
			closestCar = car;
		}
	}
	// Avoid getting to close from other cars
	closestCarDistance -= SAFE_DISTANCE_BETWEEN_CARS;
	if (closestCarDistance >= 0.0)
		return (false);

	if (outExtraDistance)
		*outExtraDistance = -closestCarDistance;
	return (true);
}

bool Car::IsNextTileAnIntersection(const IntVector2D& currentTrackTilePosition, const IntVector2D& trackTileDirectionVector) const
{
	// In fact we check two tiles ahead because otherwise we get too close from the intersection and other car may see us as an obstacle
	return (m_Track.GetTrackChar(currentTrackTilePosition + trackTileDirectionVector * 2) == INTERSECTION);
}

bool Car::IsNextIntersectionRedLightForMe(char trackDirectionChar) const
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

int Car::GetTraficLightModuloIndex(const char trackDirectionChar) const
{
	// We assume that the traffic light is always a 4 way intersection (2 input, 2 output)
	// and that it's either on diagonal or normal (no mix)
	// note that the index are either 0 or 2 that's because we want a delay to allow the car to go through before allowing the other car to go through
	if (trackDirectionChar == RIGHT_DOWN || trackDirectionChar == LEFT_UP)
		return 0;
	else if (trackDirectionChar == UP_RIGHT || trackDirectionChar == DOWN_LEFT)
		return 2;
	else if (trackDirectionChar == RIGHT || trackDirectionChar == LEFT)
		return 0;
	else if (trackDirectionChar == UP || trackDirectionChar == DOWN)
		return 2;

	assert(false);
}

Vector2D Car::FindNextDirection(const IntVector2D& currentTrackTilePosition) const
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

char Car::GetDirectionChar() const
{
	auto trackPosition = m_Track.MapPositionOnTrack(m_Position);
	char trackChar = m_Track.GetTrackChar(trackPosition);
	if (trackChar == INTERSECTION)
		return (m_LastTrackDirection);
	return (trackChar);
}