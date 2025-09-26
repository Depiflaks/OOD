#pragma once
#include "Observer.h"
#include <climits>
#include <iostream>
#include <memory>

// struct WeatherInfo
// {
// 	double m_temperature = 0;
// 	double m_humidity = 0;
// 	double m_pressure = 0;
// };

// class WeatherData;

// class Display : public AbstractObserver<WeatherInfo>
// {
// public:
//     Display(std::weak_ptr<WeatherData> weatherData)
//         : AbstractObserver<WeatherInfo>(std::static_pointer_cast<ISubject<WeatherInfo>>(weatherData.lock()))
//     {
//     }

// private:
//     void Update() override
//     {
//         auto subject = GetSubject();
//         auto data = subject.lock()->GetData();

//         std::cout << "Current Temp " << data.m_temperature << std::endl;
//         std::cout << "Current Hum " << data.m_humidity << std::endl;
//         std::cout << "Current Pressure " << data.m_pressure << std::endl;
//         std::cout << "----------------" << std::endl;
//     }
// };

// class PressureStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
// {
// public:
//     PressureStatsDisplay(std::weak_ptr<WeatherData> weatherData)
//         : AbstractStatsObserver<WeatherInfo, double>(std::static_pointer_cast<ISubject<WeatherInfo>>(weatherData.lock()))
//     {
//     }

// private:
//     void Update() override
//     {
//         auto subject = GetSubject();
//         auto data = subject.lock()->GetData();
//         UpdateStatistics(data.m_pressure);
//         PrintValues("Pressure");
//     }
// };

// class HumStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
// {
// public:
//     HumStatsDisplay(std::weak_ptr<WeatherData> weatherData)
//         : AbstractStatsObserver<WeatherInfo, double>(std::static_pointer_cast<ISubject<WeatherInfo>>(weatherData.lock()))
//     {
//     }

// private:
//     void Update() override
//     {
//         auto subject = GetSubject();
//         auto data = subject.lock()->GetData();
//         UpdateStatistics(data.m_humidity);
//         PrintValues("Hum");
//     }
// };

// class TemperatureStatsDisplay : public AbstractStatsObserver<WeatherInfo, double>
// {
// public:
//     TemperatureStatsDisplay(std::weak_ptr<WeatherData> weatherData)
//         : AbstractStatsObserver<WeatherInfo, double>(std::static_pointer_cast<ISubject<WeatherInfo>>(weatherData.lock()))
//     {
//     }

// private:
//     void Update() override
//     {
//         auto subject = GetSubject();
//         auto data = subject.lock()->GetData();
//         UpdateStatistics(data.m_temperature);
//         PrintValues("Temp");
//     }
// };

// class WeatherData : public Subject<WeatherInfo>
// {
// public:
// 	double GetTemperature() const
// 	{
// 		return m_temperature;
// 	}

// 	double GetHumidity() const
// 	{
// 		return m_humidity;
// 	}

// 	double GetPressure() const
// 	{
// 		return m_pressure;
// 	}

// 	void MeasurementsChanged()
// 	{
// 		NotifyObservers();
// 	}

// 	void SetMeasurements(double temp, double humidity, double pressure)
// 	{
// 		m_humidity = humidity;
// 		m_temperature = temp;
// 		m_pressure = pressure;

// 		MeasurementsChanged();
// 	}

// 	WeatherInfo GetData() const override
// 	{
// 		WeatherInfo info;
// 		info.m_temperature = GetTemperature();
// 		info.m_humidity = GetHumidity();
// 		info.m_pressure = GetPressure();
// 		return info;
// 	}

// private:
// 	double m_temperature = 0.0;
// 	double m_humidity = 0.0;
// 	double m_pressure = 760.0;
// };
