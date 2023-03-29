#include "Track.h"
#include "Car.h"

void FigureEightTrack::GetTrackCopy(char outTrack[s_TrackHeight][s_TrackWidth]) const
{
	for (uint8_t y = 0; y < s_TrackHeight; y++)
	{
		for (uint8_t x = 0; x < s_TrackWidth; x++)
		{
			outTrack[y][x] = m_Track[y][x];
		}
	}
}

char FigureEightTrack::GetTrackChar(IntVector2D position) const
{
	if (position.x >= 0.0f && position.x < s_TrackWidth
		&& position.y >= 0.0f && position.y < s_TrackHeight)
		return (m_Track[position.y][position.x]);
	return (' ');
}

bool FigureEightTrack::IsRoad(char character) const
{
	if (character == UP || character == UP_RIGHT || character == RIGHT || character == RIGHT_DOWN
		|| character == DOWN || character == DOWN_LEFT || character == LEFT || character == LEFT_UP)
		return (true);
	return (false);
}

IntVector2D FigureEightTrack::MapOntoTrack(const Vector2D& pos) const
{
	return (IntVector2D(
		static_cast<int>(std::floor(pos.x)),
		static_cast<int>(std::floor(pos.y)))
		);
}

Vector2D FigureEightTrack::GetSpawnPoint() const
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