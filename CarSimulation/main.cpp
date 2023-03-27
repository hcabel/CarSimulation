#include <iostream>
#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <math.h>
#include <assert.h>

#define CARS_AMOUNT 20

constexpr float MinimumCarsAcceleration = 0.1f;
constexpr float MinimumCarsMaxSpeed = 0.25f;

struct CustomFloat
{
	float value;

	CustomFloat() : value(0) {}
	CustomFloat(float value) : value(value) {}
	CustomFloat(int value) : value(static_cast<float>(value)) {}

	operator float() const { return (value); }
	operator int() const { return (static_cast<int>(std::floor(value))); }
	operator bool() const { return (value != 0); }
	operator long long() const { return (static_cast<long long>(std::floor(value))); }

	CustomFloat operator+(const CustomFloat& other) const { return (CustomFloat{ value + other.value }); }
	CustomFloat operator+(const float& other) const { return (CustomFloat{ value + other }); }
	CustomFloat operator-(const CustomFloat& other) const { return (CustomFloat{ value - other.value }); }
	CustomFloat operator-(const float& other) const { return (CustomFloat{ value - other }); }
	CustomFloat operator-() const { return (CustomFloat{ -value }); }
	CustomFloat operator*(const CustomFloat& other) const { return (CustomFloat{ value * other.value }); }
	CustomFloat operator*(const float& other) const { return (CustomFloat{ value * other }); }
	CustomFloat operator/(const CustomFloat& other) const { return (CustomFloat{ value / other.value }); }
	bool operator==(const CustomFloat& other) const { return (value == other.value); }
	bool operator==(const int other) const { return (value == other); }
	bool operator==(const float other) const { return (value == other); }

	CustomFloat& operator+=(const CustomFloat& other) { value += other.value; return (*this); }
	CustomFloat& operator-=(const CustomFloat& other) { value -= other.value; return (*this); }
	CustomFloat& operator*=(const CustomFloat& other) { value *= other.value; return (*this); }
	CustomFloat& operator/=(const CustomFloat& other) { value /= other.value; return (*this); }

	bool operator<(const CustomFloat& other) const { return (value < other.value); }
	bool operator>(const CustomFloat& other) const { return (value > other.value); }
	bool operator<=(const CustomFloat& other) const { return (value <= other.value); }
	bool operator>=(const CustomFloat& other) const { return (value >= other.value); }
	bool operator<(const int other) const { return (value < other); }
	bool operator>(const int other) const { return (value > other); }
	bool operator<=(const int other) const { return (value <= other); }
	bool operator>=(const int other) const { return (value >= other); }
	bool operator<(const uint8_t other) const { return (value < other); }
	bool operator>(const uint8_t other) const { return (value > other); }
	bool operator<=(const uint8_t other) const { return (value <= other); }
	bool operator>=(const uint8_t other) const { return (value >= other); }
	bool operator<(const float other) const { return (value < other); }
	bool operator>(const float other) const { return (value > other); }
	bool operator<=(const float other) const { return (value <= other); }
	bool operator>=(const float other) const { return (value >= other); }

	bool operator!=(const CustomFloat& other) const { return (value != other.value); }
	bool operator!=(const int other) const { return (value != other); }
	bool operator!=(const uint8_t other) const { return (value != other); }

	CustomFloat Decimal() const
	{
		return (value - std::floor(value));
	}
	bool Equal(const CustomFloat other, const float precission) const
	{
		return (std::abs(this->value - other.value) < precission);
	}
};

std::ostream& operator<<(std::ostream& os, const CustomFloat& vector)
{
	os << vector.value;
	return (os);
}

struct Vector2D
{
	CustomFloat x;
	CustomFloat y;

	Vector2D() : x(0), y(0) {}
	Vector2D(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
	Vector2D(float x, float y) : x(x), y(y) {}
	Vector2D(CustomFloat x, CustomFloat y) : x(x), y(y) {}

	Vector2D operator+(const Vector2D& other) const
	{
		return (Vector2D{ x + other.x, y + other.y });
	}
	Vector2D operator-(const Vector2D& other) const
	{
		return (Vector2D{ x - other.x, y - other.y });
	}
	Vector2D operator-(const float other) const
	{
		return (Vector2D{ x - other, y - other });
	}
	Vector2D operator-() const
	{
		return (Vector2D{ -x, -y });
	}
	Vector2D operator*(const Vector2D& other) const
	{
		return (Vector2D{ x * other.x, y * other.y });
	}
	Vector2D operator*(const CustomFloat& other) const
	{
		return (Vector2D{ x * other, y * other });
	}
	Vector2D operator*(const float& other) const
	{
		return (Vector2D{ x * other, y * other });
	}
	Vector2D operator/(const Vector2D& other) const
	{
		return (Vector2D{ x / other.x, y / other.y });
	}
	bool operator==(const Vector2D& other) const
	{
		return (x == other.x && y == other.y);
	}
	bool operator!=(const Vector2D& other) const
	{
		return (x != other.x || y != other.y);
	}

	Vector2D Round(const float precision = 0.0f) const
	{
		if (precision == 0.0f)
			return (Vector2D{ std::round(x.value), std::round(y.value) });
		return (Vector2D{ std::round(x.value / precision) * precision, std::round(y.value / precision) * precision });
	}
	Vector2D Normalize() const
	{
		CustomFloat length = std::sqrt(x * x + y * y);
		if (length == 0.0f)
			return (Vector2D{ 0, 0 });
		return (Vector2D{ x / length, y / length });
	}
	float Length() const
	{
		return (std::sqrt(x * x + y * y));
	}
	float Dot(const Vector2D& other) const
	{
		return (x * other.x + y * other.y);
	}
	float Angle(const Vector2D& other) const
	{
		return (std::acos(Dot(other) / (Length() * other.Length())));
	}
};

std::ostream& operator<<(std::ostream& os, const Vector2D& vector)
{
	os << "(x" << vector.x << ", y" << vector.y << ")";
	return (os);
}

#define NO_DIRECTION Vector2D(0.0f, 0.0f)

// Direction of the track
#define UP 'U'
#define UP_RIGHT 'E'
#define RIGHT 'D'
#define RIGHT_DOWN 'C'
#define DOWN 'X'
#define DOWN_LEFT 'Z'
#define LEFT 'A'
#define LEFT_UP 'Q'
#define INTERSECTION '*'

static Vector2D GetDirectionVector(char direction)
{
	switch (direction)
	{
	case UP: return (Vector2D{ 0, -1 });
	case UP_RIGHT: return (Vector2D{ 1, -1 }); 
	case RIGHT: return (Vector2D{ 1, 0 });
	case RIGHT_DOWN: return (Vector2D{ 1, 1 }); 
	case DOWN: return (Vector2D{ 0, 1 });
	case DOWN_LEFT: return (Vector2D{ -1, 1 }); 
	case LEFT: return (Vector2D{ -1, 0 });
	case LEFT_UP: return (Vector2D{ -1, -1 });
	case INTERSECTION: return (Vector2D{ 0, 0 });
	default: return (Vector2D{ 0, 0 });
	}
}

/**
 * The track.
 * Define where the cars should spawn, and where they can go.
 */
class FigureEightTrack
{
public:
	constexpr static uint8_t s_TrackHeight = 14;
	constexpr static uint8_t s_TrackWidth = 29;

public:
	// Get a copy of the Track has a 2d array
	void GetTrackCopy(char outTrack[s_TrackHeight][s_TrackWidth]) const
	{
		for (uint8_t y = 0; y < s_TrackHeight; y++)
		{
			for (uint8_t x = 0; x < s_TrackWidth; x++)
			{
				outTrack[y][x] = m_Track[y][x];
			}
		}
	}
	char GetTrackChar(Vector2D position) const
	{
		if (position.x >= 0.0f && position.x < s_TrackWidth
			&& position.y >= 0.0f && position.y < s_TrackHeight)
			return (m_Track[position.y][position.x]);
		return (' ');
	}

	/* Tell if the given position is a road or not */
	bool IsRoad(Vector2D position) const { return (IsRoad(m_Track[position.y][position.x])); }
	/** Tell if the given character is a road or not */
	bool IsRoad(char character) const
	{
		if (character == UP || character == UP_RIGHT || character == RIGHT || character == RIGHT_DOWN
			|| character == DOWN || character == DOWN_LEFT || character == LEFT || character == LEFT_UP)
			return (true);
		return (false);
	}
	
	Vector2D SnapPosition(const Vector2D& pos) const
	{
		return (Vector2D{ std::floor(pos.x), std::floor(pos.y) });
	}

public:
	const uint8_t GetTrackWidth() const { return (s_TrackWidth); }
	const uint8_t GetTrackHeight() const { return (s_TrackHeight); }

	/** Get a random Spawn point, if the point is not a round we look for the next road */
	Vector2D GetSpawnPoint()
	{
		int randX = std::rand() % s_TrackWidth;
		int randY = std::rand() % s_TrackHeight;
		Vector2D startPosition = Vector2D(randX, randY);
		while (!IsRoad(startPosition))
		{
			startPosition.x += 1;
			if (startPosition.x >= s_TrackWidth)
			{
				startPosition.x = 0;
				startPosition.y += 1;
				if (startPosition.y >= s_TrackHeight)
					startPosition.y = 0;
			}
		}
		// center the spawn point to the middle of the tile
		startPosition.x += 0.5f;
		startPosition.y += 0.5f;
		return (startPosition);
	}

	Vector2D GetTrackDirection(const Vector2D& position) const
	{
		char character = m_Track[position.y][position.x];
		if (character != ' ')
			return (GetDirectionVector(character));
		return (NO_DIRECTION);
	}

private:
#define _UP UP
#define _UR UP_RIGHT
#define _RI RIGHT
#define _RD RIGHT_DOWN
#define _DO DOWN
#define _DL DOWN_LEFT
#define _LE LEFT
#define _LU LEFT_UP
#define _XX INTERSECTION
	// The ASCII track
	const char m_Track[s_TrackHeight][s_TrackWidth] = {
		{ ' ', ' ', ' ', ' ', ' ', _RI, _RI, _RI, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _LE, _LE, _LE, ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', _UR, _RI, _RI, _RI, _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, _LE, _LE, _LE, _LU, ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', _UR, _UR, ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ', ' ' },
		{ ' ', ' ', _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ' },
		{ ' ', _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ' },
		{ _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU },
		{ _UP, _UP, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _XX, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UP, _UP },
		{ _UP, _UP, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _XX, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UP, _UP },
		{ _UP, _UP, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UP, _UP },
		{ ' ', _LU, _LU, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UR, _UR, ' ' },
		{ ' ', ' ', _LU, _LU, ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', _UR, _UR, ' ', ' ' },
		{ ' ', ' ', ' ', _LU, _LU, ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', _UR, _UR, ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', _LU, _LU, _LE, _LE, _LE, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RI, _RI, _RI, _UR, _UR, ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', _LU, _LE, _LE, _LE, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RI, _RI, _RI, _UR, ' ', ' ', ' ', ' ', ' ' }
	};
#undef _UP
#undef _UR
#undef _RI
#undef _RD
#undef _DO
#undef _DL
#undef _LE
#undef _LU
};

/**
 * Cars that will ride onto the track.
 */
class Cars
{

public:
	Cars(const FigureEightTrack& track, uint8_t number, Vector2D spawnPoint, float acceleration = -1, float maxSpeed = -1)
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

	void Update()
	{
		// accelerate the car
		m_Speed = std::min(m_MaxSpeed, m_Speed + m_Acceleration * m_MaxSpeed);

		m_ForwardVector = FindNewDirection();

		Vector2D trackTilePosition = m_Track.SnapPosition(m_Position);
		char trackChar = m_Track.GetTrackChar(trackTilePosition);
		Vector2D trackVector = GetDirectionVector(trackChar == INTERSECTION ? m_Direction : trackChar);
		if (m_Track.GetTrackChar(trackTilePosition + Vector2D(0.5f, 0.5f) + trackVector) == INTERSECTION)
		{
			// check if traffic light truned on for this car
			int evenOrOdd = (trackChar == RIGHT_DOWN ? 0 : 1);

			// check whether or the the traffic light is on
			constexpr int lightSwitchDurationInSeconds = 5;
			int secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			int secondsInCurrentMinute = secondsSinceEpoch % 60;
			// stop the car if the light is off (TODO: implement deceleration instead of instant stop)
			if ((secondsInCurrentMinute / lightSwitchDurationInSeconds) % 2 == evenOrOdd)
				m_Speed = 0;
		}


		// Update the position of the car
		m_Position = m_Position + (m_ForwardVector * m_Speed);
	}

	bool Collide(Vector2D position) const
	{
		float carSize = 0.25f;
		return (m_Position - position).Length() < carSize;
	}

private:
	Vector2D FindNewDirection()
	{
		char newDirection = m_Track.GetTrackChar(m_Track.SnapPosition(m_Position));

		// Change the direction unless it's an INTERSECTION
		if (newDirection != INTERSECTION)
			m_Direction = newDirection;

		// Convert char direction to a vector
		Vector2D directionVector = GetDirectionVector(m_Direction);
		if (directionVector == NO_DIRECTION)
			return (directionVector);

		// Find target point
		Vector2D targetPointDirection;
		Vector2D targetPoint = m_Position;
		float forward = 0;
		float angle = 180.0f;
		while (std::abs(angle) >= 45.0f || (targetPoint - m_Position).Length() <= m_Speed / 2)
		{
			targetPoint = m_Track.SnapPosition(m_Position + directionVector * forward) + Vector2D(0.5f, 0.5f);
			switch (m_Direction)
			{
			case UP:
				targetPoint.y -= 0.5f;
				break;
			case UP_RIGHT:
				targetPoint.y -= 0.5f;
				targetPoint.x += 0.5f;
				break;
			case RIGHT:
				targetPoint.x += 0.5f;
				break;
			case RIGHT_DOWN:
				targetPoint.y += 0.5f;
				targetPoint.x += 0.5f;
				break;
			case DOWN:
				targetPoint.y += 0.5f;
				break;
			case DOWN_LEFT:
				targetPoint.y += 0.5f;
				targetPoint.x -= 0.5f;
				break;
			case LEFT:
				targetPoint.x -= 0.5f;
				break;
			case LEFT_UP:
				targetPoint.y -= 0.5f;
				targetPoint.x -= 0.5f;
				break;
			default:
				break;
			}
			targetPointDirection = targetPoint - m_Position;
			angle = targetPointDirection.Angle(m_ForwardVector);
			forward++;
		}
		return (targetPointDirection.Normalize());
	}

public:
	Vector2D GetPosition() const { return (m_Position); }
	uint8_t GetNumber() const { return (m_Number); }
	char GetDisplayNumber() const { return (static_cast<char>(m_Number + static_cast<uint8_t>('0'))); }

private:
	/** Reference onto the track that the cars is currently driving onto */
	const FigureEightTrack& m_Track;
	/** Id of the car */
	const uint8_t m_Number;
	/** Position of the car */

	/** Position of the car */
	Vector2D m_Position;
	/** unit vector representing where the car is heading */
	Vector2D m_ForwardVector;
	/** char representing the general direction your should go to follow the track */
	char m_Direction;
	/* Car max speed, (between 0 -> 1) */
	float m_MaxSpeed;
	/* Car current speed (between 0 -> 1) */
	float m_Speed = 0.0f;
	/* Car acceleration relative to max speed (.1 acc equal to + .05 speed if maxspeed = 0.5) */
	float m_Acceleration;

};
/**
 * Render the game state onto the console, using ASCII characters.
 */
class AsciiRenderer
{

public:
	AsciiRenderer()
	{
		m_Buffer.resize(FigureEightTrack::s_TrackHeight);
		for (auto& line : m_Buffer)
			line.resize(FigureEightTrack::s_TrackWidth);
	}

public:
	void Render(const FigureEightTrack& track, const std::array<Cars*, CARS_AMOUNT>& cars)
	{
		for (auto& line : m_Buffer)
		{
			line.clear();
			line.resize(FigureEightTrack::s_TrackWidth, ' ');
		}
		// Clear the screen
		std::system("cls");

		// duplicate the track
		char trackArray[FigureEightTrack::s_TrackHeight][FigureEightTrack::s_TrackWidth];
		track.GetTrackCopy(trackArray);
		for (uint8_t y = 0; y < FigureEightTrack::s_TrackHeight; y++)
			for (uint8_t x = 0; x < FigureEightTrack::s_TrackWidth; x++)
				m_Buffer[y][x] = convertDirectionToDisplayChar(trackArray[y][x]);

		// Draw the cars onto the trackArray
		for (const Cars* car : cars)
		{
			Vector2D carPosition = track.SnapPosition(car->GetPosition());
			char carNumber = car->GetDisplayNumber(); 
			m_Buffer[static_cast<int>(carPosition.y)][static_cast<int>(carPosition.x)] = carNumber;
		}

		// print zoom level (0.1 per char)
		Vector2D carPos = Vector2D(FigureEightTrack::s_TrackWidth / 2.0f, FigureEightTrack::s_TrackHeight / 2.0f).Round(0.1f);
		float zoomSizeX = 14;
		float zoomSizeY = 6;
		float zoomStep = 0.25f;
		int bufferYLineIndex = 0;
		for (CustomFloat y = carPos.y - zoomSizeY; y < carPos.y + zoomSizeY; y += zoomStep)
		{
			for (CustomFloat x = carPos.x - zoomSizeX; x < carPos.x + zoomSizeX; x += zoomStep)
			{
				char toDisplay;
				Vector2D pos = Vector2D(x, y);
				int collideCarIndex = -1;
				for (int i = 0; i < cars.size(); i++)
				{
					if (cars[i]->Collide(pos))
					{
						collideCarIndex = i;
						break;
					}
				}

				if (collideCarIndex != -1)
					toDisplay = cars[collideCarIndex]->GetDisplayNumber();
				/*else if (pos.y.Decimal().Equal(1.0f, 0.025f) || pos.y.Decimal().Equal(0.0f, 0.025f)
					|| pos.x.Decimal().Equal(1.0f, 0.025f) || pos.x.Decimal().Equal(0.0f, 0.025f))
				{
					toDisplay = '+';
				}*/
				else
				{
					char dir = track.GetTrackChar(track.SnapPosition(pos));
					toDisplay = convertDirectionToDisplayChar(dir);
				}

				m_Buffer[bufferYLineIndex].push_back(toDisplay);
			}
			bufferYLineIndex++;
			if (bufferYLineIndex >= m_Buffer.size())
			{
				m_Buffer.resize(m_Buffer.size() + 1);
				// fill with empty spaces
				m_Buffer.back().resize(FigureEightTrack::s_TrackWidth, ' ');
			}
		}

		// print the all buffer in one shot to avoid flickering (add \n at the end of each line)
		int allocationSize = 0;
		for (auto& line : m_Buffer)
			allocationSize += line.size() + 1;
		std::string buffer;
		buffer.reserve(allocationSize);
		for (auto& line : m_Buffer)
		{
			buffer.append(line.begin(), line.end());
			buffer.push_back('\n');
		}
		std::cout << buffer;
	}
private:
	char convertDirectionToDisplayChar(char dir)
	{
		if (dir == LEFT || dir == RIGHT)
			return ('-');
		else if (dir == UP || dir == DOWN)
			return ('|');
		else if (dir == LEFT_UP || dir == RIGHT_DOWN)
			return ('\\');
		else if (dir == UP_RIGHT || dir == DOWN_LEFT)
			return ('/');
		else if (dir == INTERSECTION)
			return ('X');
		return (' ');
	}

private:
		std::vector<std::vector<char>> m_Buffer;
};

int main()
{
	// set rand seed otherwise will always have the same RNG
	std::srand(time(nullptr));

	std::array<Cars*, CARS_AMOUNT> cars;
	FigureEightTrack track;
	AsciiRenderer renderer;

	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = track.GetSpawnPoint();
		cars[i] = new Cars(track, i + 1, spawnPoint);
	}

	while (true)
	{
		renderer.Render(track, cars);

		for (Cars* car : cars)
		{
			car->Update();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}