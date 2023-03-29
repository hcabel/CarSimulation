#include "Car.h"
#include "Track.h"
#include "Renderer.h"

#include <array>
#include <chrono>
#include <thread>
#include <memory>
#include <assert.h>

#define REFRESH_DURATION_IN_MS std::chrono::milliseconds(100)

static Vector2D GetUniqueSpawnPoint(ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars)
{
	uint16_t attempt = 0;

	Vector2D spawnPoint;
	bool isSpawnPointUnique;
	do
	{
		isSpawnPointUnique = true;
		spawnPoint = track.GetSpawnPoint();

		for (auto car : cars)
		{
			if (car != nullptr && car->GetPosition() == spawnPoint)
			{
				isSpawnPointUnique = false;
				break;
			}
		}

		attempt++;
		if (attempt >= 10000)
			assert(false);
	} while (isSpawnPointUnique == false);

	return spawnPoint;
}

int main()
{
	// set rand seed otherwise will always have the same RNG
	std::srand(time(nullptr));

	std::array<std::shared_ptr<Car>, CARS_AMOUNT> cars;

#if SELECTED_MAP == 0
	ATrack track = FigureEightTrack();
#else
	ATrack track = MultiIntersectionTrack();
#endif

	AsciiRenderer renderer;

	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = GetUniqueSpawnPoint(track, cars);

		cars[i] = std::make_shared<Car>(track, i, spawnPoint);
		track.RegisterNewCarOnTrack(cars[i]);
	}

	std::chrono::nanoseconds time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::nanoseconds timeTakenToLoop = {};
	std::chrono::nanoseconds currentTime = {};

	// Main loop
	while (true)
	{
		renderer.Render(track, cars);

		for (auto car : cars)
			car->Move();

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		timeTakenToLoop = currentTime - time;
		time = currentTime;
		if (timeTakenToLoop < std::chrono::duration_cast<std::chrono::nanoseconds>(REFRESH_DURATION_IN_MS))
		{
			std::this_thread::sleep_for(
				std::chrono::duration_cast<std::chrono::nanoseconds>(REFRESH_DURATION_IN_MS) - timeTakenToLoop
			);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}