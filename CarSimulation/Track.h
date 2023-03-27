#pragma once

#include "Defines.h"

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
	char GetTrackChar(IntVector2D position) const
	{
		if (position.x >= 0.0f && position.x < s_TrackWidth
			&& position.y >= 0.0f && position.y < s_TrackHeight)
			return (m_Track[position.y][position.x]);
		return (' ');
	}

	/* Tell if the given position is a road or not */
	bool IsRoad(IntVector2D position) const { return (IsRoad(m_Track[position.y][position.x])); }
	/** Tell if the given character is a road or not */
	bool IsRoad(char character) const
	{
		if (character == UP || character == UP_RIGHT || character == RIGHT || character == RIGHT_DOWN
			|| character == DOWN || character == DOWN_LEFT || character == LEFT || character == LEFT_UP)
			return (true);
		return (false);
	}

	IntVector2D MapOntoTrack(const Vector2D& pos) const
	{
		return (IntVector2D(
			static_cast<int>(std::floor(pos.x)),
			static_cast<int>(std::floor(pos.y)))
		);
	}

public:
	const uint8_t GetTrackWidth() const { return (s_TrackWidth); }
	const uint8_t GetTrackHeight() const { return (s_TrackHeight); }

	/** Get a random Spawn point, if the point is not a road we look for the next road, once the track find we add 0.5 to x and y to center the spawn point onto the tile */
	Vector2D GetSpawnPoint()
	{
		int randX = std::rand() % s_TrackWidth;
		int randY = std::rand() % s_TrackHeight;
		Vector2D startPosition = Vector2D(static_cast<float>(randX), static_cast<float>(randY));
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

	IntVector2D GetTrackDirection(const IntVector2D& position) const
	{
		char character = m_Track[position.y][position.x];
		if (character != ' ')
			return (GetDirectionVector(character));
		return (CENTER_VECTOR);
	}

private:
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
	// The ASCII track
	const char m_Track[s_TrackHeight][s_TrackWidth] = {
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
