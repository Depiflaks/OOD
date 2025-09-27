#pragma once
#include "Observer.h"
#include "Subject.h"
#include <climits>
#include <iostream>

struct OutsideWeatherInfo
{
	double m_temperature = 0;
	double m_humidity = 0;
	double m_pressure = 0;
};

struct InsideTemperatureInfo
{
	double m_temperature = 0;
};

class OutsideWeatherData;

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

class InsideTemperatureData : public Subject<InsideTemperatureInfo>
{
public:
	InsideTemperatureInfo GetData() const override
	{
		InsideTemperatureInfo info;
		info.m_temperature = m_temperature;
		return info;
	}

private:
	double m_temperature = 0.0;
};

class OutsideWeatherData : public Subject<OutsideWeatherInfo>
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

	OutsideWeatherInfo GetData() const override
	{
		OutsideWeatherInfo info;
		info.m_temperature = GetTemperature();
		info.m_humidity = GetHumidity();
		info.m_pressure = GetPressure();
		return info;
	}

private:
	double m_temperature = 0.0;
	double m_humidity = 0.0;
	double m_pressure = 760.0;
};
