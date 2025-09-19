#pragma once
#include "Observer.h"
#include <climits>
#include <iostream>

struct WeatherInfo
{
	double temperature = 0;
	double humidity = 0;
	double pressure = 0;
};

class Display : public IObserver<WeatherInfo>
{
private:
	void Update(WeatherInfo const& data) override
	{
		std::cout << "Current Temp " << data.temperature << std::endl;
		std::cout << "Current Hum " << data.humidity << std::endl;
		std::cout << "Current Pressure " << data.pressure << std::endl;
		std::cout << "----------------" << std::endl;
	}
};

class PressureStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
{
private:
	void Update(WeatherInfo const& data) override
	{
		UpdateStatistics(data.pressure);
		PrintValues("Pressure");
	}
};

class HumStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
{
private:
	void Update(WeatherInfo const& data) override
	{
		UpdateStatistics(data.humidity);
		PrintValues("Hum");
	}
};

class TemperatureStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
{
private:
	void Update(WeatherInfo const& data) override
	{
		UpdateStatistics(data.temperature);
		PrintValues("Temp");
	}
};

class WeatherData : public Observable<WeatherInfo>
{
public:
	double GetTemperature() const
	{
		return m_temperature;
	}

	double GetHumidity() const
	{
		return m_humidity;
	}

	double GetPressure() const
	{
		return m_pressure;
	}

	void MeasurementsChanged()
	{
		NotifyObservers();
	}

	void SetMeasurements(double temp, double humidity, double pressure)
	{
		m_humidity = humidity;
		m_temperature = temp;
		m_pressure = pressure;

		MeasurementsChanged();
	}

protected:
	WeatherInfo GetChangedData() const override
	{
		WeatherInfo info;
		info.temperature = GetTemperature();
		info.humidity = GetHumidity();
		info.pressure = GetPressure();
		return info;
	}

private:
	double m_temperature = 0.0;
	double m_humidity = 0.0;
	double m_pressure = 760.0;
};
