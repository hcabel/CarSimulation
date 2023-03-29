#pragma once

#include "Defines.h"

#include <vector>
#include <memory>

class Car;

/**
 * Class that contain all the properties / member to manage a track
 * /!\ does not have any track /!\
 */
class ATrack
{

public:
	ATrack(std::vector<std::vector<char>>&& map)
		: m_TrackMap(std::move(map)), m_Width(m_TrackMap[0].size()), m_Height(m_TrackMap.size())
	{}

public:
	/**
	 * Copy the track into the 2d vector given in argument.
	 * Note: we assume that you reserve enough space yourself.
	 */
	void CopyTrack(std::vector<std::vector<char>>& outTrack) const;
	/* Convert a position to a position of a tile in the track map*/
	IntVector2D MapPositionOnTrack(const Vector2D& position) const;
	/* return whether or not the giver char is a road */
	bool IsRoad(char c) const;
	/* return whether or not the given position is a road */
	bool IsHereARoad(const IntVector2D& pos) const;
	bool IsHereARoad(const Vector2D& pos) const;
	/* return whether or not the given position is out of track map bounds */
	bool IsHereInMapBounds(const IntVector2D& pos) const;

public:
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	/* return the track char at the given position, or '\0' if out of bound */
	char GetTrackChar(const IntVector2D& pos) const;
	/* Return all the cars that has been register has driving onto the track */
	const std::vector<std::weak_ptr<Car>> GetCarsOnTrack() const { return m_CarsRegisterOnTrack; }
	/** Get a random Spawn point, if the point is not a road we look for the next road, once the track find we add 0.5 to x and y to center the spawn point onto the tile */
	Vector2D GetSpawnPoint() const;

	void RegisterNewCarOnTrack(std::weak_ptr<Car> car) { m_CarsRegisterOnTrack.push_back(car); }

protected:
	/** The track itself, made of char that represent in which direction the car should go */
	const std::vector<std::vector<char>> m_TrackMap;
	/** All the cars registered has driving on this track */
	std::vector<std::weak_ptr<Car>> m_CarsRegisterOnTrack;

	int m_Width;
	int m_Height;
};

// Create 3 char long alias for the direction char macro (easier to use)
#define _U_ UP
#define _UR UP_RIGHT
#define _R_ RIGHT
#define _RD RIGHT_DOWN
#define _D_ DOWN
#define _DL DOWN_LEFT
#define _L_ LEFT
#define _LU LEFT_UP
#define _X_ INTERSECTION

/**
 * Figure Eight Track
 */
class FigureEightTrack : public ATrack
{

public:
	FigureEightTrack()
		: ATrack({
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
		})
	{}
};

/**
 * A custom track that I made to show how easy is it to make any kind of track with this system.
 */
class MultiIntersectionTrack : public ATrack
{

public:
	MultiIntersectionTrack()
		: ATrack({
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
		})
	{}
};

// Undef the macros aliases
#undef _U_
#undef _UR
#undef _R_
#undef _RD
#undef _DO
#undef _DL
#undef _L_
#undef _LU
#undef _X_
