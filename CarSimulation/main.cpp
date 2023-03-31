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
	std::chrono::nanoseconds sleepTime = {};

	// Thread loop
	while (true)
	{
		car->Move();

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		timeTakenToLoop = currentTime - time;
		sleepTime = std::chrono::nanoseconds(std::max(0ll, (std::chrono::duration_cast<std::chrono::nanoseconds>(THREAD_REFRESH_DURATION) - timeTakenToLoop).count()));
		std::this_thread::sleep_for(sleepTime);
		time = currentTime - sleepTime;
	}
}

static int MainLoopGameThread(const ATrack& track, const std::array<std::shared_ptr<Car>, CARS_AMOUNT>& cars)
{
	AsciiRenderer renderer;

	std::chrono::nanoseconds time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::chrono::nanoseconds timeTakenToLoop = {};
	std::chrono::nanoseconds currentTime = {};
	std::chrono::nanoseconds sleepTime = {};

	// Main loop
	while (true)
	{

#if MULTI_THREADING == 0
		for (auto car : cars)
			car->Move();
#endif

		renderer.Render(track, cars);

		// check if no cars are overlapping
		for (int i = 0; i < CARS_AMOUNT; i++)
		{
			for (int j = i + 1; j < CARS_AMOUNT; j++)
			{
				if (cars[i]->IsColliding(cars[j]))
				{
					std::cout << "Collision between car '" << cars[i]->GetDisplayChar() << "' and car '" << cars[j]->GetDisplayChar() << "'" << std::endl;
					break;
				}
			}

			// make sure it's on the track
			if (track.IsHereARoad(track.MapPositionOnTrack(cars[i]->GetPosition())) == false)
			{
				std::cout << "Car '" << cars[i]->GetDisplayChar() << "' is off the track" << std::endl;
			}
		}

		currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());
		timeTakenToLoop = currentTime - time;
		sleepTime = std::chrono::nanoseconds(std::max(0ll, (std::chrono::duration_cast<std::chrono::nanoseconds>(MAIN_THREAD_REFRESH_DURATION) - timeTakenToLoop).count()));
		std::this_thread::sleep_for(sleepTime);
		time = currentTime - sleepTime;
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

	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = GetUniqueSpawnPoint(track, cars);

		cars[i] = std::make_shared<Car>(track, i, spawnPoint);
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