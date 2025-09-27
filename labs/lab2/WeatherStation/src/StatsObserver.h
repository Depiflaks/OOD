#ifndef STATSOBSERVER_H
#define STATSOBSERVER_H

#include "Concepts.h"
#include "Observer.h"
#include "WeatherData.h"
#include <cmath>

// template <typename ContextType, SuitableValueType ValueType>
template <typename ContextType, typename ValueType>
class MonoAbstractStatsObserver : public MonoAbstractObserver<ContextType>
{
public:
	MonoAbstractStatsObserver(const IObservable<ContextType>& observable)
		: MonoAbstractObserver<ContextType>{ observable }
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


#endif /* STATSOBSERVER_H */

