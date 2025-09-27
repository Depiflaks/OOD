#pragma once

#include "./Concepts.h"
#include "./Subject.h"
#include <iostream>
#include <limits>
#include <string_view>
#include <sys/stat.h>
#include <vector>

template <typename T>
class IObserver
{
public:
	virtual ~IObserver() = default;
	virtual void Update(const IObservable<T>& observable) = 0;
};

template <typename T>
class AbstractObserver : public IObserver<T>
{
public:
	AbstractObserver(const IObservable<T>& observable)
		: m_observable(static_cast<const void*>(&observable))
	{
	}

protected:
	void ChangeObservable(const IObservable<T>& observable)
	{
		m_observable = static_cast<const void*>(&observable);
	}

	void* GetObservable() const
	{
		return const_cast<void*>(m_observable);
	}

	bool CheckSameObservable(const IObservable<T>& observable) const
	{
		return m_observable == static_cast<const void*>(&observable);
	}

private:
	const void* m_observable;
};

template <typename T>
class MonoAbstractObserver : public AbstractObserver<T>
{
public:
	MonoAbstractObserver(const IObservable<T>& observable)
		: AbstractObserver<T>(observable)
	{
	}

	void ChangeObservable(const IObservable<T>& observable)
	{
		AbstractObserver<T>::ChangeObservable(observable);
	}

	void Update(const IObservable<T>& observable) override
	{
		if (this->CheckSameObservable(observable))
		{
			UpdateObserver(observable);
		}
	}

protected:
	virtual void UpdateObserver(const IObservable<T>& observable) = 0;
};

template <typename T1, typename T2>
class DuoAbstractObserver : public AbstractObserver<T1>
	, public AbstractObserver<T2>
{
public:
	DuoAbstractObserver(const IObservable<T1>& first, const IObservable<T2>& second)
		: AbstractObserver<T1>(first)
		, AbstractObserver<T2>(second)
	{
	}

	void ChangeObservables(const IObservable<T1>& first, const IObservable<T2>& second)
	{
		AbstractObserver<T1>::ChangeObservable(first);
		AbstractObserver<T2>::ChangeObservable(second);
	}

	void Update(const IObservable<T1>& observable) override
	{
		if (AbstractObserver<T1>::CheckSameObservable(observable))
		{
			UpdateFirst(observable);
		}
	}

	void Update(const IObservable<T2>& observable) override
	{
		if (AbstractObserver<T2>::CheckSameObservable(observable))
		{
			UpdateSecond(observable);
		}
	}

protected:
	virtual void UpdateFirst(const IObservable<T1>& observable) = 0;
	virtual void UpdateSecond(const IObservable<T2>& observable) = 0;
};

template <typename ContextType, SuitableValueType ValueType>
class MonoAbstractStatsObserver : public MonoAbstractObserver<ContextType>
{
public:
	MonoAbstractStatsObserver(const IObservable<ContextType>& observable)
		: AbstractObserver<ContextType>{ observable }
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
		if (m_countAcc == 0)
		{
			return;
		}
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
