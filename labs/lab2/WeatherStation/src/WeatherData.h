#pragma once
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

template <>
class MonoAbstractStatsObserver<OutsideWeatherInfo, WindInfo> : public MonoAbstractObserver<OutsideWeatherInfo>
{
public:
	MonoAbstractStatsObserver(const IObservable<OutsideWeatherInfo>& observable)
		: MonoAbstractObserver<OutsideWeatherInfo>{ observable }
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

class Display : public MonoAbstractObserver<OutsideWeatherInfo>
{
public:
	Display(const IObservable<OutsideWeatherInfo>& weatherData)
		: MonoAbstractObserver<OutsideWeatherInfo>(weatherData)
	{
	}

protected:
	void UpdateObserver(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Current Temp " << data.m_temperature << "\n";
		std::cout << "Current Hum " << data.m_humidity << "\n";
		std::cout << "Current Pressure " << data.m_pressure << "\n";
		std::cout << "----------------\n";
	}
};

class PressureStatsDisplay : public MonoAbstractStatsObserver<OutsideWeatherInfo, double>
{
public:
	PressureStatsDisplay(const IObservable<OutsideWeatherInfo>& weatherData)
		: MonoAbstractStatsObserver<OutsideWeatherInfo, double>(weatherData)
	{
	}

protected:
	void UpdateObserver(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_pressure);
		PrintValues("Pressure");
	}
};

class HumStatsDisplay : public MonoAbstractStatsObserver<OutsideWeatherInfo, double>
{
public:
	HumStatsDisplay(const IObservable<OutsideWeatherInfo>& weatherData)
		: MonoAbstractStatsObserver<OutsideWeatherInfo, double>(weatherData)
	{
	}

protected:
	void UpdateObserver(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_humidity);
		PrintValues("Hum");
	}
};

class TemperatureStatsDisplay : public MonoAbstractStatsObserver<OutsideWeatherInfo, double>
{
public:
	TemperatureStatsDisplay(const IObservable<OutsideWeatherInfo>& weatherData)
		: MonoAbstractStatsObserver<OutsideWeatherInfo, double>(weatherData)
	{
	}

protected:
	void UpdateObserver(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_temperature);
		PrintValues("Temp");
	}
};

class DuoTemperatureDisplay : public DuoAbstractObserver<OutsideWeatherInfo, InsideTemperatureInfo>
{
public:
	DuoTemperatureDisplay(const IObservable<OutsideWeatherInfo>& inside, const IObservable<InsideTemperatureInfo>& outside)
		: DuoAbstractObserver<OutsideWeatherInfo, InsideTemperatureInfo>(inside, outside)
	{
	}

protected:
	void UpdateFirst(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Outside Temp: " << data.m_temperature << "\n";
	}

	void UpdateSecond(const IObservable<InsideTemperatureInfo>& observable) override
	{
		auto data = observable.GetData();
		std::cout << "Inside Temp: " << data.m_temperature << "\n";
	}
};

class WindStatsDisplay : public MonoAbstractStatsObserver<OutsideWeatherInfo, WindInfo>
{
public:
	WindStatsDisplay(const IObservable<OutsideWeatherInfo>& weatherData)
		: MonoAbstractStatsObserver<OutsideWeatherInfo, WindInfo>(weatherData)
	{
	}

protected:
	void UpdateObserver(const IObservable<OutsideWeatherInfo>& observable) override
	{
		auto data = observable.GetData();
		UpdateStatistics(data.m_wind);
		PrintValues("Wind");
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

	void SetMeasurements(double temperature)
	{
		m_temperature = temperature;
		NotifyObservers();
	}

private:
	double m_temperature = 0.0;
};

class OutsideWeatherData : public Subject<OutsideWeatherInfo>
{
public:
	void MeasurementsChanged()
	{
		NotifyObservers();
	}

	void SetMeasurements(double temp, double humidity, double pressure, double windSpeed, double windDirection)
	{
		m_humidity = humidity;
		m_temperature = temp;
		m_pressure = pressure;
		m_wind.direction = windDirection;
		m_wind.speed = windSpeed;

		MeasurementsChanged();
	}

	OutsideWeatherInfo GetData() const override
	{
		OutsideWeatherInfo info;
		info.m_temperature = m_temperature;
		info.m_humidity = m_humidity;
		info.m_pressure = m_pressure;
		info.m_wind = m_wind;
		return info;
	}

private:
	double m_temperature = 0.0;
	double m_humidity = 0.0;
	double m_pressure = 760.0;
	WindInfo m_wind{};
};
