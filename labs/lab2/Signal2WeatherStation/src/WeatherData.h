#pragma once
#include "Enum/SubscribeType.h"
#include "Observer.h"
#include "StatsObserver.h"
#include "Subject.h"
#include <climits>
#include <iostream>

class OutsideWeatherData;

struct WindInfo
{
	double speed = 0;
	double direction = 0;
};

struct OutsideWeatherInfo
{
	double m_temperature = 0;
	double m_humidity = 0;
	double m_pressure = 0;
	WindInfo m_wind{};
};

struct InsideTemperatureInfo
{
	double m_temperature = 0;
};

struct HospitalSensorsInfo
{
	OutsideWeatherInfo m_outsideWeather;
	int m_patientPulse;
};

template <>
class MonoAbstractStatsObserver<HospitalSensorsInfo, WindInfo> : public MonoAbstractObserver<HospitalSensorsInfo>
{
public:
	MonoAbstractStatsObserver(const IObservable<HospitalSensorsInfo>& observable)
		: MonoAbstractObserver<HospitalSensorsInfo>{ observable }
	{
	}

protected:
	void UpdateStatistics(const WindInfo& value)
	{
		if (m_minSpeed > value.speed)
		{
			m_minSpeed = value.speed;
		}
		if (m_maxSpeed < value.speed)
		{
			m_maxSpeed = value.speed;
		}

		m_accSpeed += value.speed;

		double rad = value.direction * M_PI / 180.0;
		m_sinSum += std::sin(rad);
		m_cosSum += std::cos(rad);

		++m_countAcc;
	}

	void PrintValues(std::string_view valueName)
	{
		if (m_countAcc == 0)
		{
			return;
		}

		std::cout << "Max " << valueName << " speed: " << m_maxSpeed << std::endl;
		std::cout << "Min " << valueName << " speed: " << m_minSpeed << std::endl;
		std::cout << "Average " << valueName << " speed: " << (m_accSpeed / m_countAcc) << std::endl;

		double avgSin = m_sinSum / m_countAcc;
		double avgCos = m_cosSum / m_countAcc;
		double avgDirection = std::atan2(avgSin, avgCos) * 180.0 / M_PI;
		if (avgDirection < 0)
		{
			avgDirection += 360.0;
		}

		std::cout << "Average " << valueName << " direction: " << avgDirection << "°" << std::endl;

		std::string directionName = GetDirectionName(avgDirection);
		std::cout << "Average " << valueName << " direction name: " << directionName << std::endl;
		std::cout << "----------------" << std::endl;
	}

private:
	double m_minSpeed = std::numeric_limits<double>::infinity();
	double m_maxSpeed = -std::numeric_limits<double>::infinity();
	double m_accSpeed = 0;
	double m_sinSum = 0;
	double m_cosSum = 0;
	unsigned m_countAcc = 0;

	std::string GetDirectionName(double degrees) const
	{
		if (degrees >= 337.5 || degrees < 22.5)
			return "North";
		if (degrees >= 22.5 && degrees < 67.5)
			return "Northeast";
		if (degrees >= 67.5 && degrees < 112.5)
			return "East";
		if (degrees >= 112.5 && degrees < 157.5)
			return "Southeast";
		if (degrees >= 157.5 && degrees < 202.5)
			return "South";
		if (degrees >= 202.5 && degrees < 247.5)
			return "Southwest";
		if (degrees >= 247.5 && degrees < 292.5)
			return "West";
		return "Northwest";
	}
};

class WeatherDisplay : public MonoAbstractObserver<HospitalSensorsInfo>
{
public:
	WeatherDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractObserver<HospitalSensorsInfo>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Current Temp " << data.m_outsideWeather.m_temperature << "\n";
		std::cout << "Current Hum " << data.m_outsideWeather.m_humidity << "\n";
		std::cout << "Current Pressure " << data.m_outsideWeather.m_pressure << "\n";
		std::cout << "----------------\n";
	}
};
;

class PressureStatsDisplay : public MonoAbstractStatsObserver<HospitalSensorsInfo, double>
{
public:
	PressureStatsDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractStatsObserver<HospitalSensorsInfo, double>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_outsideWeather.m_pressure);
		PrintValues("Pressure");
	}
};

class HumStatsDisplay : public MonoAbstractStatsObserver<HospitalSensorsInfo, double>
{
public:
	HumStatsDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractStatsObserver<HospitalSensorsInfo, double>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_outsideWeather.m_humidity);
		PrintValues("Hum");
	}
};

class TemperatureStatsDisplay : public MonoAbstractStatsObserver<HospitalSensorsInfo, double>
{
public:
	TemperatureStatsDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractStatsObserver<HospitalSensorsInfo, double>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_outsideWeather.m_temperature);
		PrintValues("Temp");
	}
};

class DuoTemperatureDisplay : public DuoAbstractObserver<HospitalSensorsInfo, InsideTemperatureInfo>
{
public:
	DuoTemperatureDisplay(const IObservable<HospitalSensorsInfo>& hospitalSensors, const IObservable<InsideTemperatureInfo>& insideTemp)
		: DuoAbstractObserver<HospitalSensorsInfo, InsideTemperatureInfo>(hospitalSensors, insideTemp)
	{
	}

protected:
	void UpdateFirst(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Hospital Outside Temp: " << data.m_outsideWeather.m_temperature << "\n";
	}

	void UpdateSecond(const IObservable<InsideTemperatureInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Inside Temp: " << data.m_temperature << "\n";
	}
};

class WindStatsDisplay : public MonoAbstractStatsObserver<HospitalSensorsInfo, WindInfo>
{
public:
	WindStatsDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractStatsObserver<HospitalSensorsInfo, WindInfo>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_outsideWeather.m_wind);
		PrintValues("Wind");
	}
};

class PulseStatsDisplay : public MonoAbstractStatsObserver<HospitalSensorsInfo, int>
{
public:
	PulseStatsDisplay(const IObservable<HospitalSensorsInfo>& sensorsData)
		: MonoAbstractStatsObserver<HospitalSensorsInfo, int>(sensorsData)
	{
	}

protected:
	void UpdateObserver(const IObservable<HospitalSensorsInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_patientPulse);
		PrintValues("Pulse");
	}
};

class InsideTemperatureData : public Subject<InsideTemperatureInfo>
{
public:
	InsideTemperatureInfo GetData() const override
	{
		InsideTemperatureInfo info;
		info.m_temperature = m_temperature;
		return info;
	}

	void UpdateTemperature(double temperature)
	{
		m_temperature = temperature;
		Notify(SubscribeType::TEMPERATURE_CHANGE);
	}

private:
	double m_temperature = 0.0;
};

class HospitalSensorsData : public Subject<HospitalSensorsInfo>
{
public:
	void MeasurementsChanged(SubscribeType type)
	{
		Notify(type);
	}

	void UpdateTemperature(double temp) {
		m_outsideWeather.m_temperature = temp;
		MeasurementsChanged(SubscribeType::TEMPERATURE_CHANGE);
	}

	void UpdateHumidity(double humidity) {
		m_outsideWeather.m_humidity = humidity;
		MeasurementsChanged(SubscribeType::HUMIDITY_CHANGE);
	}

	void UpdatePressure(double pressure) {
		m_outsideWeather.m_pressure = pressure;
		MeasurementsChanged(SubscribeType::PRESSURE_CHANGE);
	}

	void UpdateWind(double windSpeed, double windDirection) {
		m_outsideWeather.m_wind = WindInfo{windSpeed, windDirection};
		MeasurementsChanged(SubscribeType::WIND_CHANGE);
	}

	void UpdatePulse(int patientPulse) {
		m_patientPulse = patientPulse;
		MeasurementsChanged(SubscribeType::PULSE_CHANGE);
	}

	void SetMeasurements(double temp, double humidity, double pressure, double windSpeed, double windDirection, int patientPulse)
	{
		m_outsideWeather.m_temperature = temp;
		m_outsideWeather.m_humidity = humidity;
		m_outsideWeather.m_pressure = pressure;
		m_outsideWeather.m_wind.speed = windSpeed;
		m_outsideWeather.m_wind.direction = windDirection;
		m_patientPulse = patientPulse;

		MeasurementsChanged(SubscribeType::TEMPERATURE_CHANGE);
		MeasurementsChanged(SubscribeType::PRESSURE_CHANGE);
		MeasurementsChanged(SubscribeType::HUMIDITY_CHANGE);
		MeasurementsChanged(SubscribeType::WIND_CHANGE);
		MeasurementsChanged(SubscribeType::PULSE_CHANGE);
	}

	HospitalSensorsInfo GetData() const override
	{
		HospitalSensorsInfo info;
		info.m_outsideWeather = m_outsideWeather;
		info.m_patientPulse = m_patientPulse;
		return info;
	}

private:
	OutsideWeatherInfo m_outsideWeather;
	int m_patientPulse = 0;
};
