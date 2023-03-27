#include "Car.h"
#include "Track.h"
#include "Renderer.h"

#include <array>
#include <chrono>
#include <thread>

int main()
{
	// set rand seed otherwise will always have the same RNG
	std::srand(time(nullptr));

	std::array<Car*, CARS_AMOUNT> cars;
	FigureEightTrack track;
	AsciiRenderer renderer;

	for (int i = 0; i < CARS_AMOUNT; i++)
	{
		Vector2D spawnPoint = track.GetSpawnPoint();
		cars[i] = new Car(track, i + 1, spawnPoint);
	}

	while (true)
	{
		renderer.Render(track, cars);

		for (Car* car : cars)
		{
			car->Move();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// delete cars
	for (auto& car : cars)
		delete car;

	return 0;
}