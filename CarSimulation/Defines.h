#pragma once

#include "Vector2D.h"
#include "IntVector2D.h"

/* SETTINGS **********************************************/

// -- SELECT HOW MANY CARS YOU WANT --
#define CARS_AMOUNT 8

// -- SELECT A TRACK --
// 0 = Figure eight track map
// 1 = Custom map
#define SELECTED_MAP 0

// -- SELECT A DRIVING MODE --
// 0 = No colision
// 1 = Colision (taffic jam simulator)
// 2 = switch lane
#define DRIVING_MODE 2

// Turn on and off the multi threading
#define MULTI_THREADING 1

#define THREAD_REFRESH_DURATION std::chrono::milliseconds(100)
// I recommend not to go bellow 100 ms because the console is not fast enough to render the game
#define MAIN_THREAD_REFRESH_DURATION std::chrono::milliseconds(100)

/* CAR SETTINGS */

// The car a circle and this define control the radius of the circle.
#define CAR_SIZE_RADIUS 0.2f
// Add safe distance, to avoid cars to get to close to each other
// even though with this value to 0 the cars wont crash but sometime they might not be able to open there door ^^
// between 0 -> car max speed (greater and it wont be taken for consideration anyway)
#define SAFE_DISTANCE_BETWEEN_CARS 0.1f
// Acceleration is relative to the max speed (0 -> 1)
#define CAR_MIN_ACCELERATION 0.1f
#define CAR_MAX_ACCELERATION 1.0f
// Max speed (0 -> 1)
#define CAR_MIN_MAXSPEED 0.1f
#define CAR_MAX_MAXSPEED 0.2f
#define CAR_MAX_STEERINGANGLE_DEGREE 45.0f

/* FOR CODE READABILIY *************************************/

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
