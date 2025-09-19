#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stdexcept>
#include <string_view>
#include <sys/stat.h>

#include "./Concepts.h"

template <typename T>
class IObserver;

template <typename T>
class ISubject
{
public:
	virtual ~ISubject() = default;
	virtual void RegisterObserver(std::weak_ptr<IObserver<T>> observer) = 0;
	virtual void NotifyObservers() = 0;
	virtual void RemoveObserver(std::weak_ptr<IObserver<T>> observer) = 0;
	virtual T GetData() const = 0;
};

template <class T>
class Subject : public ISubject<T>
{
public:
	typedef IObserver<T> ObserverType;

	void RegisterObserver(std::weak_ptr<ObserverType> observer) override
	{
		m_observers.insert(observer);
	}

	void NotifyObservers() override
	{
		T data = GetData();
		for (auto& observer : m_observers)
		{
			if (std::shared_ptr<ObserverType> observerPtr = observer.lock())
			{
				observerPtr->Update();
			}
			else
			{
				RemoveObserver(observer);
			}
		}
	}

	void RemoveObserver(std::weak_ptr<ObserverType> observer) override
	{
		m_observers.erase(observer);
	}

	virtual T GetData() const = 0;

private:
	std::set<std::weak_ptr<ObserverType>, std::owner_less<std::weak_ptr<ObserverType>>> m_observers;

	static void CheckObserverExist(std::weak_ptr<ObserverType> observer)
	{
		if (observer.expired())
		{
			throw std::runtime_error{ "Observer no longer exists" };
		}
	}
};

template <typename T>
class IObserver
{
public:
	virtual void Update() = 0;
	virtual ~IObserver() = default;
};

template <typename T>
class AbstractObserver : public IObserver<T>
	, public std::enable_shared_from_this<IObserver<T>>
{
protected:
	AbstractObserver(std::weak_ptr<ISubject<T>> subject)
		: m_subject(subject)
	{
		CheckSubjectExist(subject);

		std::shared_ptr<ISubject<T>> subjectPtr = subject.lock();
		subjectPtr->RegisterObserver(this->shared_from_this());
	}

	std::weak_ptr<ISubject<T>> GetSubject() const
	{
		CheckSubjectExist(m_subject);
		return m_subject;
	}

private:
	std::weak_ptr<ISubject<T>> m_subject;

	static void CheckSubjectExist(std::weak_ptr<ISubject<T>> subject)
	{
		if (subject.expired())
		{
			throw std::runtime_error{ "Subject no longer exists" };
		}
	}
};

template <typename ContextType, SuitableValueType ValueType>
class AbstractStatsObserver : public AbstractObserver<ContextType>
{
public:
	AbstractStatsObserver(std::weak_ptr<ISubject<ContextType>> subject)
		: AbstractObserver<ContextType>{ subject }
	{
	}

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
