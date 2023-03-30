#include "Car.h"
#include "Track.h"
#include "Renderer.h"

#include <array>
#include <chrono>
#include <thread>
#include <memory>
#include <assert.h>

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

void ThreadFunction(std::shared_ptr<Car> car)
{
	std::chrono::nanoseconds time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::nanoseconds timeTakenToLoop = {};
	std::chrono::nanoseconds currentTime = {};

	// Thread loop
	while (true)
	{
		car->Move();

		std::this_thread::sleep_for(
			THREAD_REFRESH_DURATION
		);
	}
}

static int MainLoopGameThread(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars)
{
	AsciiRenderer renderer;

	std::chrono::nanoseconds time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::nanoseconds timeTakenToLoop = {};
	std::chrono::nanoseconds currentTime = {};

	// Main loop
	while (true)
	{
		renderer.Render(track, cars);

#if MULTI_THREADING == 0
		for (auto car : cars)
			car->Move();
#endif

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		timeTakenToLoop = currentTime - time;
		time = currentTime;
		std::this_thread::sleep_for(
			MAIN_THREAD_REFRESH_DURATION
		);
	}
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

	cars[0] = std::make_shared<Car>(track, 0, track.GetSpawnPoint(), 1.0f, 1.0f);

	for (int i = 1; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = GetUniqueSpawnPoint(track, cars);
		// Vector2D spawnPoint = Vector2D(7.5 + -2.0 * i, 0.5);

		cars[i] = std::make_shared<Car>(track, i, spawnPoint);
		// cars[i] = std::make_shared<Car>(track, i, spawnPoint, 1, 0.50 + 0.2 * i);
		track.RegisterNewCarOnTrack(cars[i]);
	}

#if MULTI_THREADING
	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		std::thread threadProcess(ThreadFunction, cars[i]);
		threadProcess.detach();
	}
#endif

	MainLoopGameThread(track, cars);

	return 0;
}