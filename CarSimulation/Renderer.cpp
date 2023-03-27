#include "Renderer.h"

void AsciiRenderer::Render(const FigureEightTrack& track, const std::array<Car*, CARS_AMOUNT>& cars)
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
	for (const Car* car : cars)
	{
		IntVector2D carPosition = track.MapOntoTrack(car->GetPosition());
		char carNumber = car->GetDisplayNumber();
		m_Buffer[carPosition.y][carPosition.x] = carNumber;
	}

	// print zoom level (0.1 per char)
	Vector2D carPos = Vector2D(FigureEightTrack::s_TrackWidth / 2.0f, FigureEightTrack::s_TrackHeight / 2.0f).Round(0.1f);
	float zoomSizeX = 15;
	float zoomSizeY = 7;
	float zoomStep = 0.25f;
	int bufferYLineIndex = 0;
	for (float y = carPos.y - zoomSizeY; y < carPos.y + zoomSizeY; y += zoomStep)
	{
		for (float x = carPos.x - zoomSizeX; x < carPos.x + zoomSizeX; x += zoomStep)
		{
			char toDisplay;
			Vector2D pos = { x, y };
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
				char dir = track.GetTrackChar(track.MapOntoTrack(pos));
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
	size_t allocationSize = 0;
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

char AsciiRenderer::convertDirectionToDisplayChar(char dir)
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