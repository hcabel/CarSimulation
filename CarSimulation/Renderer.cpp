#include "Renderer.h"

void AsciiRenderer::Render(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars)
{
	for (auto& line : m_Buffer)
	{
		line.clear();
		line.resize(track.GetWidth(), ' ');
	}
	// Clear the screen
	std::system("cls");

	float zoomSteps = 1.0 / 5.0;
	Vector2D zoomCenter = Vector2D(track.GetWidth() / 2.0f, track.GetHeight() / 2.0f);
		// cars[0]->GetPosition().Round(zoomSteps);
	float zoomWidth = track.GetWidth() ;
	float zoomHeight = track.GetHeight();

	// calculate the alocation for the buffer
	int bufferWidth = track.GetWidth() + std::ceil(zoomWidth / zoomSteps) + 1;
	int bufferHeight = track.GetHeight() + std::ceil(zoomHeight / zoomSteps) + 1;

	// resize the buffer
	m_Buffer.resize(bufferHeight);
	for (auto& line : m_Buffer)
		line.resize(bufferWidth, ' ');

	DrawMapOnBuffer(track);
	DrawCarsOnBuffer(cars);
	DrawCloseUp(track, cars, zoomCenter, zoomWidth, zoomHeight, zoomSteps);
	DrawBufferOnScreen();
}

void AsciiRenderer::DrawMapOnBuffer(const ATrack& track)
{
	// duplicate the track on the map buffer
	m_MapBuffer.resize(track.GetHeight());
	for (auto& line : m_MapBuffer)
	{
		line.clear();
		line.resize(track.GetWidth(), ' ');
	}
	track.CopyTrack(m_MapBuffer);

	// Draw the track
	for (uint8_t y = 0; y < track.GetHeight(); y++)
		for (uint8_t x = 0; x < track.GetWidth(); x++)
			m_Buffer[y][x] = convertDirectionToDisplayChar(m_MapBuffer[y][x]);
}

void AsciiRenderer::DrawCarsOnBuffer(const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars)
{
	// Draw the cars
	for (auto car : cars)
	{
		IntVector2D carPosition = car->GetPosition().Round(0.1f);
		char carNumber = car->GetDisplayChar();
		if (carPosition.y >= 0 && carPosition.y < m_Buffer.size()
			&& carPosition.x >= 0 && carPosition.x < m_Buffer[0].size())
			m_Buffer[carPosition.y][carPosition.x] = carNumber;
		else
			std::cout << "Unable to draw: " << car->GetId() << std::endl;
	}
}

void AsciiRenderer::DrawCloseUp(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars, const Vector2D& center, float width, float height, float stepping)
{
	// print zoom level (0.1 per char)
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;

	int bufferYLineIndex = 0;
	for (float y = center.y - halfHeight; y < center.y + halfHeight; y += stepping)
	{
		int bufferXColumnIndex = track.GetWidth();
		for (float x = center.x - halfWidth; x < center.x + halfWidth; x += stepping)
		{
			char toDisplay;
			Vector2D pos = { x, y };

			// Check if colliding with any of the cars
			int collideCarIndex = -1;
			for (int i = 0; i < cars.size(); i++)
			{
				if (cars[i]->IsColliding(pos))
				{
					collideCarIndex = i;
					break;
				}
			}

			// if it collide with a cars draw his id on the map
			if (collideCarIndex != -1)
				toDisplay = cars[collideCarIndex]->GetDisplayChar();
			// if it doesn't collide with a map draw the road display char
			else
			{
				char dir = track.GetTrackChar(track.MapPositionOnTrack(pos));
				toDisplay = convertDirectionToDisplayChar(dir);
			}

			m_Buffer[bufferYLineIndex][bufferXColumnIndex] = toDisplay;
			bufferXColumnIndex++;
		}
		bufferYLineIndex++;
		if (bufferYLineIndex >= m_Buffer.size())
		{
			m_Buffer.resize(m_Buffer.size() + 1);
			// fill with empty spaces
			m_Buffer.back().resize(track.GetWidth(), ' ');
		}
	}
}

void AsciiRenderer::DrawBufferOnScreen()
{
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
		return ('.');
	else if (dir == UP || dir == DOWN)
		return ('.');
	else if (dir == LEFT_UP || dir == RIGHT_DOWN)
		return ('.');
	else if (dir == UP_RIGHT || dir == DOWN_LEFT)
		return ('.');
	else if (dir == INTERSECTION)
		return ('.');
	return (' ');
}