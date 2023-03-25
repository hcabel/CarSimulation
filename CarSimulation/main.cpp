#include <iostream>
#include <array>
#include <thread>
#include <chrono>
#include <math.h>
#include <assert.h>

#define CARS_AMOUNT 1

#define LOG(Format, ...) \
	std::printf(Format, __VA_ARGS__); \
	std::printf("\n");

#define CHECKF(Condition, Format, ...) \
	if (!(Condition)) \
	{ \
		LOG(Format, __VA_ARGS__); \
		assert(false); \
	}

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
	CustomFloat operator-(const CustomFloat& other) const { return (CustomFloat{ value - other.value }); }
	CustomFloat operator-() const { return (CustomFloat{ -value }); }
	CustomFloat operator*(const CustomFloat& other) const { return (CustomFloat{ value * other.value }); }
	CustomFloat operator*(const float& other) const { return (CustomFloat{ value * other }); }
	CustomFloat operator/(const CustomFloat& other) const { return (CustomFloat{ value / other.value }); }
	bool operator==(const CustomFloat& other) const { return (value == other.value); }

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

	bool operator!=(const CustomFloat& other) const { return (value != other.value); }
	bool operator!=(const int other) const { return (value != other); }
	bool operator!=(const uint8_t other) const { return (value != other); }
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

	Vector2D Round() const { return (Vector2D{ static_cast<int>(std::round(x)), static_cast<int>(std::round(y)) }); }
	Vector2D Normalize() const
	{
		CustomFloat length = std::sqrt(x * x + y * y);
		return (Vector2D{ x / length, y / length });
	}
};

std::ostream& operator<<(std::ostream& os, const Vector2D& vector)
{
	os << "(" << vector.x << ", " << vector.y << ")";
	return (os);
}

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
	case UP_RIGHT: return (Vector2D{ 0.707107f, -0.707107f }); // Nornalize
	case RIGHT: return (Vector2D{ 1, 0 });
	case RIGHT_DOWN: return (Vector2D{ 0.707107f, 0.707107f }); // Nornalize
	case DOWN: return (Vector2D{ 0, 1 });
	case DOWN_LEFT: return (Vector2D{ -0.707107f, 0.707107f }); // Nornalize
	case LEFT: return (Vector2D{ -1, 0 });
	case LEFT_UP: return (Vector2D{ -0.707107f, -0.707107f }); // Nornalize
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
		startPosition.x += 0.5f;
		startPosition.y += 0.5f;
		return (startPosition);
	}

	Vector2D GetTrackDirection(const Vector2D& position) const
	{
		char character = m_Track[position.y][position.x];
		if (character != ' ')
			return (GetDirectionVector(character));
		return (Vector2D(0, 0));
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
#define _UP
#define _UR
#define _RI
#define _RD
#define _DO
#define _DL
#define _LE
#define _LU
};

/**
 * Cars that will ride onto the track.
 */
class Cars
{

public:
	Cars(const FigureEightTrack& track, uint8_t number, Vector2D spawnPoint, float speed = -1)
		: m_Track(track), m_Number(number), m_Position(spawnPoint), m_Speed(speed == -1 ? static_cast<float>(std::rand()) / RAND_MAX : speed)
	{}

	void Update()
	{
		Vector2D newDirection = FindNewDirection();

		// Correct the newDirection to keep the car at the center of the track
		Vector2D

		m_Position = m_Position + m_ForwardVector * m_Speed;
		std::cout << "Car " << static_cast<int>(m_Number) << " is at " << m_Position << std::endl;
	}

private:
	Vector2D FindNewDirection() const
	{
		Vector2D newDirection = m_Track.GetTrackDirection(m_Position.Round());
		// if the direction is 0, 0 this mean that the track does not provide any direction so we keep going forward
		if (newDirection == Vector2D(0, 0)) {
			return (m_ForwardVector);
		}
		return (newDirection);
	}

public:
	Vector2D GetPosition() const { return (m_Position); }
	uint8_t GetNumber() const { return (m_Number); }

private:
	/** Reference onto the track that the cars is currently driving onto */
	const FigureEightTrack& m_Track;
	/** Id of the car */
	const uint8_t m_Number;
	/** Position of the car */
	Vector2D m_Position;
	/** Direction where the car is heading */
	Vector2D m_ForwardVector;
	/* Car speed */
	float m_Speed;
};
/**
 * Render the game state onto the console, using ASCII characters.
 */
class AsciiRenderer
{

public:
	void Render(const FigureEightTrack& track, const std::array<Cars*, CARS_AMOUNT>& cars)
	{
		// Clear the screen
	//	std::system("cls");

		// duplicate the track
		char trackArray[FigureEightTrack::s_TrackHeight][FigureEightTrack::s_TrackWidth];
		track.GetTrackCopy(trackArray);
		for (uint8_t y = 0; y < FigureEightTrack::s_TrackHeight; y++)
			for (uint8_t x = 0; x < FigureEightTrack::s_TrackWidth; x++)
				trackArray[y][x] = convertDirectionToDisplayChar(trackArray[y][x]);

		// Draw the cars onto the trackArray
		for (const Cars* car : cars)
		{
			Vector2D carPosition = car->GetPosition().Round();
			char carNumber = static_cast<char>(car->GetNumber() + static_cast<uint8_t>('0'));
			trackArray[carPosition.y][carPosition.x] = carNumber;
		}

		// Print the trackArray
		for (uint8_t y = 0; y < FigureEightTrack::s_TrackHeight; y++)
		{
			for (uint8_t x = 0; x < FigureEightTrack::s_TrackWidth; x++)
				std::cout << trackArray[y][x];
			std::cout << '\n';
		}
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
		return ('.');
	}
};

int main()
{
	// set rand seed otherwise will always have the same result for out game RNG
	std::srand(time(nullptr));

	std::array<Cars*, CARS_AMOUNT> cars;
	FigureEightTrack track;
	AsciiRenderer renderer;

	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = track.GetSpawnPoint();

		cars[i] = new Cars(track, i + 1, spawnPoint, 0.1f);
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