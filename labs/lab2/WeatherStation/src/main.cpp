#include "./WeatherData.h"

int main()
{
	WeatherData wd;

	Display display;
	wd.RegisterObserver(display);

	PressureStatsDisplay pressureStatsDisplay;
	HumStatsDisplay humStatsDisplay;
	TemperatureStatsDisplay tempStatsDisplay;
	wd.RegisterObserver(pressureStatsDisplay);
	wd.RegisterObserver(humStatsDisplay);
	wd.RegisterObserver(tempStatsDisplay);

	wd.SetMeasurements(3, 0.7, 760);
	wd.SetMeasurements(4, 0.8, 761);

	wd.RemoveObserver(pressureStatsDisplay);
	wd.RemoveObserver(humStatsDisplay);
	wd.RemoveObserver(tempStatsDisplay);


	wd.SetMeasurements(10, 0.8, 761);
	wd.SetMeasurements(-10, 0.8, 761);
	return 0;
}
