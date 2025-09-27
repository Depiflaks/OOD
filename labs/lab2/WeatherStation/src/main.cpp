#include "./WeatherData.h"
#include <memory>

int main()
{
	auto hospitalData = std::make_shared<HospitalSensorsData>();
	auto insideData = std::make_shared<InsideTemperatureData>();

	auto display = std::make_shared<WeatherDisplay>(*hospitalData);
	auto pressureStats = std::make_shared<PressureStatsDisplay>(*hospitalData);
	auto humStats = std::make_shared<HumStatsDisplay>(*hospitalData);
	auto tempStats = std::make_shared<TemperatureStatsDisplay>(*hospitalData);
	auto windStats = std::make_shared<WindStatsDisplay>(*hospitalData);
	auto pulseStats = std::make_shared<PulseStatsDisplay>(*hospitalData);
	auto duoDisplay = std::make_shared<DuoTemperatureDisplay>(*hospitalData, *insideData);

	// outsideData->RegisterObserver(display);
	// outsideData->RegisterObserver(pressureStats);
	// outsideData->RegisterObserver(humStats);
	// outsideData->RegisterObserver(tempStats);
	hospitalData->RegisterObserver(pulseStats);
	hospitalData->RegisterObserver(windStats);
	hospitalData->RegisterObserver(duoDisplay);

	insideData->RegisterObserver(duoDisplay);

	std::cout << "=== First measurement ===\n";
	hospitalData->SetMeasurements(25.5, 65.0, 755.0, 5.0, 45.0, 70);
	insideData->SetMeasurements(22.0);

	std::cout << "\n=== Second measurement ===\n";
	hospitalData->SetMeasurements(28.0, 60.0, 750.0, 8.5, 90.0, 80);
	insideData->SetMeasurements(23.5);

	std::cout << "\n=== Third measurement ===\n";
	hospitalData->SetMeasurements(22.0, 70.0, 760.0, 3.2, 180.0, 90);
	insideData->SetMeasurements(21.0);

	std::cout << "\n=== Fourth measurement ===\n";
	hospitalData->SetMeasurements(18.0, 75.0, 765.0, 12.1, 270.0, 100);
	insideData->SetMeasurements(20.0);

	return 0;
}
