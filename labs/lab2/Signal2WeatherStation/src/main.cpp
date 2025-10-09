#include "./WeatherData.h"
#include "Enum/SubscribeType.h"
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

	// outsideData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, display);
	// outsideData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, pressureStats);
	// outsideData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, humStats);
	// outsideData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, tempStats);
	hospitalData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, pulseStats);
	hospitalData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, windStats);
	hospitalData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, duoDisplay);

	insideData->Subscribe(SubscribeType::TEMPERATURE_CHANGE, duoDisplay);

	std::cout << "=== First measurement ===\n";
	hospitalData->SetMeasurements(25.5, 65.0, 755.0, 5.0, 45.0, 70);
	insideData->UpdateTemperature(22.0);

	std::cout << "\n=== Second measurement ===\n";
	hospitalData->SetMeasurements(28.0, 60.0, 750.0, 8.5, 90.0, 80);
	insideData->UpdateTemperature(23.5);

	std::cout << "\n=== Third measurement ===\n";
	hospitalData->SetMeasurements(22.0, 70.0, 760.0, 3.2, 180.0, 90);
	insideData->UpdateTemperature(21.0);

	std::cout << "\n=== Fourth measurement ===\n";
	hospitalData->SetMeasurements(18.0, 75.0, 765.0, 12.1, 270.0, 100);
	insideData->UpdateTemperature(20.0);

	return 0;
}
