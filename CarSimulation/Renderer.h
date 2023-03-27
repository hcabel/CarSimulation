#pragma once

#include "Defines.h"
#include "Vector2D.h"
#include "IntVector2D.h"
#include "Car.h"
#include "Track.h"

#include <iostream>
#include <vector>
#include <array>

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
	void Render(const FigureEightTrack& track, const std::array<Car*, CARS_AMOUNT>& cars);
private:
	char convertDirectionToDisplayChar(char dir);

private:
		std::vector<std::vector<char>> m_Buffer;
};
