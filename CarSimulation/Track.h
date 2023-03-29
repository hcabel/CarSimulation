#pragma once

#include "Defines.h"

#include <vector>

class Car;

/**
 * Class that contain all the properties / member to manage a track
 * /!\ does not have any track /!\
 */
class ATrack
{

public:
	ATrack()
		: m_Height(m_TrackMap.size()), m_Width(m_TrackMap[0].size())
	{}

public:
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }

	/* Convert a position to a position of a tile in the track map*/
	IntVector2D MapPositionOnTrack(const Vector2D& position);
	/* return whether or not the giver char is a road */
	bool IsRoad(char c);
	/* return whether or not the given position is a road */
	bool IsHereARoad(IntVector2D pos);
	/* return whether or not the given position is out of track map bounds */
	bool IsHereOutOfBounds(IntVector2D pos);

	/* return the track char at the given position, or '\0' if out of bound */
	char GetTrackChar(IntVector2D pos);

	/* Return all the cars that has been register has driving onto the track */
	const std::vector<std::weak_ptr<Car>> GetCarsOnTrack() const { return m_CarsRegisterOnTrack; }

protected:
	const std::vector<std::vector<char>> m_TrackMap;
	const std::vector<std::weak_ptr<Car>> m_CarsRegisterOnTrack;

	const int m_Width;
	const int m_Height;
};

/**
 * The track.
 * Define where the cars should spawn, and where they can go.
 */
class FigureEightTrack : ATrack
{
public:
	constexpr static uint8_t s_TrackHeight = 14;
	constexpr static uint8_t s_TrackWidth = 29;

public:
	// Get a copy of the Track has a 2d array
	void GetTrackCopy(char outTrack[s_TrackHeight][s_TrackWidth]) const;
	char GetTrackChar(IntVector2D position) const;

	/* Tell if the given position is a road or not */
	bool IsRoad(IntVector2D position) const { return (IsRoad(m_Track[position.y][position.x])); }
	/** Tell if the given character is a road or not */
	bool IsRoad(char character) const;

	IntVector2D MapOntoTrack(const Vector2D& pos) const;

public:
	const uint8_t GetTrackWidth() const { return (s_TrackWidth); }
	const uint8_t GetTrackHeight() const { return (s_TrackHeight); }

	const std::vector<Car*>& GetCarsOnTrack() const { return (m_CarsOnTrack); }
	void RegisterNewCarOnTrack(Car* car) { m_CarsOnTrack.push_back(car); }

	/** Get a random Spawn point, if the point is not a road we look for the next road, once the track find we add 0.5 to x and y to center the spawn point onto the tile */
	Vector2D GetSpawnPoint() const;

	IntVector2D GetTrackDirection(const IntVector2D& position) const
	{
		char character = m_Track[position.y][position.x];
		if (character != ' ')
			return (GetDirectionVector(character));
		return (CENTER_VECTOR);
	}

private:
	std::vector<Car*> m_CarsOnTrack;

// convert enum to 3 char enum to keep the look of the track consistency
#define _U_ UP
#define _UR UP_RIGHT
#define _R_ RIGHT
#define _RD RIGHT_DOWN
#define _D_ DOWN
#define _DL DOWN_LEFT
#define _L_ LEFT
#define _LU LEFT_UP
#define _X_ INTERSECTION
	// The ASCII track, use 3char define to tell where the car should go, if you doing right you can make pretty much any map
	/*const char m_Track[s_TrackHeight][s_TrackWidth] = {
		{ ' ', ' ', ' ', ' ', ' ', _R_, _R_, _R_, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _L_, _L_, _L_, ' ', ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', _UR, _R_, _R_, _R_, _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, _L_, _L_, _L_, _LU, ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', _UR, _UR, ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ', ' ' },
		{ ' ', ' ', _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ' },
		{ ' ', _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ' },
		{ _UR, _UR, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU },
		{ _U_, _U_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _X_, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _U_, _U_ },
		{ _U_, _U_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _X_, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _U_, _U_ },
		{ _U_, _U_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _U_, _U_ },
		{ ' ', _LU, _LU, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UR, _UR, ' ' },
		{ ' ', ' ', _LU, _LU, ' ', ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', _UR, _UR, ' ', ' ' },
		{ ' ', ' ', ' ', _LU, _LU, ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', _UR, _UR, ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', _LU, _LU, _L_, _L_, _L_, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _R_, _R_, _R_, _UR, _UR, ' ', ' ', ' ', ' ' },
		{ ' ', ' ', ' ', ' ', ' ', _LU, _L_, _L_, _L_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _R_, _R_, _R_, _UR, ' ', ' ', ' ', ' ', ' ' }
	};*/
	// TEST THIS MAP:
	const char m_Track[s_TrackHeight][s_TrackWidth] = {
		{ ' ', ' ', ' ', _DL, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, ' ', ' ', ' ', ' ', ' ', ' '},
		{ ' ', ' ', _DL, _DL, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _L_, _LU, ' ', ' ', ' ', ' ', ' '},
		{ ' ', _DL, _DL, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ', ' ', ' '},
		{ _D_, _D_, ' ', ' ', _DL, _L_, _L_, ' ', ' ', ' ', ' ', _R_, _RD, ' ', ' ', ' ', ' ', ' ', _DL, _L_, _L_, _L_, ' ', ' ', _LU, _LU, ' ', ' ', ' '},
		{ _D_, _D_, ' ', _D_, _D_, _L_, _L_, _LU, ' ', ' ', _UR, _R_, _RD, _RD, ' ', ' ', ' ', _DL, _DL, _L_, _L_, _L_, _LU, ' ', ' ', _LU, _LU, ' ', ' '},
		{ _D_, _D_, ' ', _D_, _D_, ' ', _U_, _U_, ' ', _UR, _UR, ' ', ' ', _RD, _RD, ' ', _DL, _DL, ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ', _LU, _LU, ' '},
		{ _D_, _D_, ' ', _RD, _R_, _R_, _X_, _X_, _UR, _UR, ' ', ' ', ' ', ' ', _RD, _X_, _DL, ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ', ' ', _LU, _LU },
		{ _D_, _D_, ' ', ' ', _R_, _R_, _X_, _X_, _UR, ' ', ' ', ' ', ' ', ' ', _DL, _X_, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _LU, _LU, ' ', _U_, _U_ },
		{ _D_, _D_, ' ', ' ', ' ', ' ', _U_, _U_, ' ', ' ', ' ', ' ', ' ', _DL, _DL, ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', _U_, _U_, ' ', _U_, _U_},
		{ _D_, _D_, ' ', ' ', ' ', ' ', _U_, _U_, ' ', ' ', ' ', ' ', _DL, _DL, ' ', ' ', ' ', _RD, _R_, _R_, _R_, _R_, _R_, _R_, _U_, _U_, ' ', _U_, _U_},
		{ _RD, _RD, ' ', ' ', ' ', _UR, _UR, ' ', ' ', ' ', ' ', _D_, _D_, ' ', ' ', ' ', ' ', ' ', _R_, _R_, _R_, _R_, _R_, _R_, _UR, ' ', ' ', _U_, _U_},
		{ ' ', _RD, _R_, _R_, _UR, _UR, ' ', ' ', ' ', ' ', ' ', _RD, _RD, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _UR, _UR, ' '},
		{ ' ', ' ', _R_, _R_, _UR, ' ', ' ', ' ', ' ', ' ', ' ', ' ', _RD, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _UR, _UR, ' ', ' '},
		{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _R_, _UR, ' ', ' ', ' '}
	};
#undef _U_
#undef _UR
#undef _R_
#undef _RD
#undef _DO
#undef _DL
#undef _L_
#undef _LU
#undef _X_
};
