#pragma once

#include "Defines.h"
#include "Vector2D.h"
#include "IntVector2D.h"
#include "Car.h"
#include "Track.h"

#include <iostream>
#include <vector>
#include <array>
#include <memory>

/**
 * Render the game state onto the console, using ASCII characters.
 */
class AsciiRenderer
{
public:
	void Render(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars);

private:
	void DrawMapOnBuffer(const ATrack& track);
	void DrawCarsOnBuffer(const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars);
	void DrawCloseUp(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars, const Vector2D& center, float width, float height, float stepping);
	void DrawBufferOnScreen();
	char convertDirectionToDisplayChar(char dir);

private:
	std::vector<std::vector<char>> m_MapBuffer;
	std::vector<std::vector<char>> m_Buffer;
};
