#pragma once

#include <iostream>
#include <limits>
#include <set>
#include <string_view>

template <typename T>
class IObserver
{
public:
	virtual void Update(T const& data) = 0;
	virtual ~IObserver() = default;
};

template <typename T>
concept Arithmetic = requires(T a, T b) {
	requires std::is_arithmetic_v<T>;
	{ a + b } -> std::same_as<T>;
	{ a - b } -> std::same_as<T>;
	{ a * b } -> std::same_as<T>;
	{ a / b } -> std::same_as<T>;
	{ a < b } -> std::convertible_to<bool>;
	{ a > b } -> std::convertible_to<bool>;
	{ a == b } -> std::convertible_to<bool>;
};

template <typename T>
concept HasNumericLimits = requires {
	typename std::numeric_limits<T>;
	{ std::numeric_limits<T>::infinity() } -> std::convertible_to<T>;
	{ std::numeric_limits<T>::max() } -> std::convertible_to<T>;
	{ std::numeric_limits<T>::min() } -> std::convertible_to<T>;
};

template <typename T>
concept SuitableValueType = Arithmetic<T> && HasNumericLimits<T>;

template <typename ContextType, SuitableValueType ValueType>
class AbstractStatsObserver : public IObserver<ContextType>
{
protected:
	void UpdateStatistics(const ValueType& value)
	{
		if (m_minValue > value)
		{
			m_minValue = value;
		}
		if (m_maxValue < value)
		{
			m_maxValue = value;
		}
		m_accValue += value;
		++m_countAcc;
	}

	void PrintValues(std::string_view valueName)
	{
		std::cout << "Max " << valueName << ": " << m_maxValue << std::endl;
		std::cout << "Min " << valueName << ": " << m_minValue << std::endl;
		std::cout << "Average " << valueName << ": " << (m_accValue / m_countAcc) << std::endl;
		std::cout << "----------------" << std::endl;
	}

private:
	ValueType m_minValue = std::numeric_limits<ValueType>::infinity();
	ValueType m_maxValue = -std::numeric_limits<ValueType>::infinity();
	ValueType m_accValue = 0;
	unsigned m_countAcc = 0;
};

template <typename T>
class IObservable
{
public:
	virtual ~IObservable() = default;
	virtual void RegisterObserver(IObserver<T>& observer) = 0;
	virtual void NotifyObservers() = 0;
	virtual void RemoveObserver(IObserver<T>& observer) = 0;
};

template <class T>
class Observable : public IObservable<T>
{
public:
	typedef IObserver<T> ObserverType;

	void RegisterObserver(ObserverType& observer) override
	{
		m_observers.insert(&observer);
	}

	void NotifyObservers() override
	{
		T data = GetChangedData();
		for (auto& observer : m_observers)
		{
			observer->Update(data);
		}
	}

	void RemoveObserver(ObserverType& observer) override
	{
		m_observers.erase(&observer);
	}

protected:
	virtual T GetChangedData() const = 0;

private:
	std::set<ObserverType*> m_observers;
};
