#pragma once

#include "Vector2D.h"
#include "IntVector2D.h"

// -- SELECT HOW MANY CARS YOU WANT --
#define CARS_AMOUNT 10

// -- SELECT A TRACK --
// 0 or 1 (Currently only 2 map)
#define SELECTED_MAP 1

// So I didn't implement switching lane, and unless the car is fast enough to skip the next car it will get stuck behind lol
// Turn this define to 1 to see all the car in a traffic jam
#define TRAFFIC_JAM_MODE 0

constexpr float MinimumCarsAcceleration = 0.1f;
constexpr float MinimumCarsMaxSpeed = 0.25f;

// Direction of the track
#define UP 'U'
#define UP_RIGHT 'E'
#define RIGHT 'D'
#define RIGHT_DOWN 'C'
#define DOWN 'X'
#define DOWN_LEFT 'Z'
#define LEFT 'A'
#define LEFT_UP 'Q'
#define CENTER ' '
#define INTERSECTION '*'

#define UP_VECTOR IntVector2D(0, -1)
#define UP_RIGHT_VECTOR IntVector2D(1, -1)
#define RIGHT_VECTOR IntVector2D(1, 0)
#define RIGHT_DOWN_VECTOR IntVector2D(1, 1)
#define DOWN_VECTOR IntVector2D(0, 1)
#define DOWN_LEFT_VECTOR IntVector2D(-1, 1)
#define LEFT_VECTOR IntVector2D(-1, 0)
#define LEFT_UP_VECTOR IntVector2D(-1, -1)
#define CENTER_VECTOR IntVector2D(0, 0)
#define INTERSECTION_VECTOR CENTER_VECTOR

static IntVector2D GetDirectionVector(char direction)
{
	switch (direction)
	{
	case UP: return UP_VECTOR;
	case UP_RIGHT: return UP_RIGHT_VECTOR;
	case RIGHT: return RIGHT_VECTOR;
	case RIGHT_DOWN: return RIGHT_DOWN_VECTOR;
	case DOWN: return DOWN_VECTOR;
	case DOWN_LEFT: return DOWN_LEFT_VECTOR;
	case LEFT: return LEFT_VECTOR;
	case LEFT_UP: return LEFT_UP_VECTOR;
	case CENTER: return CENTER_VECTOR;
	case INTERSECTION: return INTERSECTION_VECTOR;
	}
	return CENTER_VECTOR;
}

#define CLAMP(Min, Max, Val) std::min(Max, std::max(Min, Val))
