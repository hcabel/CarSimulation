#include "Track.h"
#include "Car.h"

void ATrack::CopyTrack(std::vector<std::vector<char>>& outTrack) const
{
	for (int y = 0; y < m_Height; y++)
	{
		for (int x = 0; x < m_Width; x++)
		{
			outTrack[y][x] = m_TrackMap[y][x];
		}
	}
}

IntVector2D ATrack::MapPositionOnTrack(const Vector2D& position) const
{
	return (IntVector2D(
		static_cast<int>(std::floor(position.x)),
		static_cast<int>(std::floor(position.y)))
		);
}

bool ATrack::IsRoad(char c) const
{
	if (c == UP || c == UP_RIGHT || c == RIGHT || c == RIGHT_DOWN
		|| c == DOWN || c == DOWN_LEFT || c == LEFT || c == LEFT_UP)
		return (true);
	return (false);
}

bool ATrack::IsHereARoad(const IntVector2D& pos) const
{
	return IsRoad(GetTrackChar(pos));
}

bool ATrack::IsHereARoad(const Vector2D& pos) const
{
	return IsRoad(GetTrackChar(pos));
}

bool ATrack::IsHereInMapBounds(const IntVector2D& pos) const
{
	return (pos.x >= 0.0f && pos.x < GetWidth() && pos.y >= 0.0f && pos.y < GetHeight());
}

char ATrack::GetTrackChar(const IntVector2D& pos) const
{
	if (IsHereInMapBounds(pos))
		return (m_TrackMap[pos.y][pos.x]);
	return (' ');
}

Vector2D ATrack::GetSpawnPoint() const
{
	int randX = std::rand() % GetWidth();
	int randY = std::rand() % GetHeight();
	Vector2D startPosition = Vector2D(static_cast<float>(randX), static_cast<float>(randY));
	while (IsHereARoad(startPosition) == false)
	{
		startPosition.x += 1;
		if (startPosition.x >= GetWidth())
		{
			startPosition.x = 0;
			startPosition.y += 1;
			if (startPosition.y >= GetHeight())
				startPosition.y = 0;
		}
	}
	// center the spawn point to the middle of the tile
	startPosition.x += 0.5f;
	startPosition.y += 0.5f;
	return (startPosition);
}