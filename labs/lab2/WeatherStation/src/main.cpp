#include "./WeatherData.h"
#include <memory>

int main()
{
	auto outsideData = std::make_shared<OutsideWeatherData>();
	auto insideData = std::make_shared<InsideTemperatureData>();

	auto display = std::make_shared<Display>(*outsideData);
	auto pressureStats = std::make_shared<PressureStatsDisplay>(*outsideData);
	auto humStats = std::make_shared<HumStatsDisplay>(*outsideData);
	auto tempStats = std::make_shared<TemperatureStatsDisplay>(*outsideData);
	auto windStats = std::make_shared<WindStatsDisplay>(*outsideData);
	auto duoDisplay = std::make_shared<DuoTemperatureDisplay>(*outsideData, *insideData);

	// outsideData->RegisterObserver(display);
	// outsideData->RegisterObserver(pressureStats);
	// outsideData->RegisterObserver(humStats);
	// outsideData->RegisterObserver(tempStats);
	outsideData->RegisterObserver(windStats);
	// outsideData->RegisterObserver(duoDisplay);

	// insideData->RegisterObserver(duoDisplay);

	std::cout << "=== First measurement ===\n";
	outsideData->SetMeasurements(25.5, 65.0, 755.0, 5.0, 45.0);
	insideData->SetMeasurements(22.0);

	std::cout << "\n=== Second measurement ===\n";
	outsideData->SetMeasurements(28.0, 60.0, 750.0, 8.5, 90.0);
	insideData->SetMeasurements(23.5);

	std::cout << "\n=== Third measurement ===\n";
	outsideData->SetMeasurements(22.0, 70.0, 760.0, 3.2, 180.0);
	insideData->SetMeasurements(21.0);

	std::cout << "\n=== Fourth measurement ===\n";
	outsideData->SetMeasurements(18.0, 75.0, 765.0, 12.1, 270.0);
	insideData->SetMeasurements(20.0);

	return 0;
}
