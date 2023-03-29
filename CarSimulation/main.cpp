#include "Car.h"
#include "Track.h"
#include "Renderer.h"

#include <array>
#include <chrono>
#include <thread>
#include <memory>

#define REFRESH_DURATION_IN_MS 100

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
		Vector2D spawnPoint = track.GetSpawnPoint();
		cars[i] = std::make_shared<Car>(track, i, spawnPoint);
		track.RegisterNewCarOnTrack(cars[i]);
	}
	while (true)
	{
		renderer.Render(track, cars);

		for (auto car : cars)
			car->Move();

		std::this_thread::sleep_for(std::chrono::milliseconds(REFRESH_DURATION_IN_MS));
	}

	return 0;
}