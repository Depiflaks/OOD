#pragma once

#include <string>
#include "IBeverage.h"

enum class CoffeePortion
{
	Standard,
	Double
};

enum class TeaSort
{
	Green,
	Black,
	Herbal
};

enum class MilkshakeSize
{
	Small,
	Medium,
	Large
};

class Coffee : public IBeverage
{
public:
	Coffee(CoffeePortion portion = CoffeePortion::Standard)
		: m_portion(portion)
	{
	}

	virtual std::string GetDescription() const override
	{
		return "Coffee";
	}

	double GetCost() const override
	{
		return 50.0;
	}

protected:
	CoffeePortion m_portion;
};

class Cappuccino : public Coffee
{
public:
	Cappuccino(CoffeePortion portion = CoffeePortion::Standard)
		: Coffee(portion)
	{
	}

	std::string GetDescription() const override final
	{
		return "Double Cappuccino";
	}

	double GetCost() const override
	{
		return (m_portion == CoffeePortion::Double) ? 120.0 : 80.0;
	}
};

class Latte : public Coffee
{
public:
	Latte(CoffeePortion portion = CoffeePortion::Standard)
		: Coffee(portion)
	{
	}

	std::string GetDescription() const override final
	{
		return "Double Latte";
	}

	double GetCost() const override
	{
		return (m_portion == CoffeePortion::Double) ? 130.0 : 90.0;
	}
};

class Tea : public IBeverage
{
public:
	Tea(TeaSort sort = TeaSort::Black)
		: m_sort(sort)
	{
	}

	std::string GetDescription() const override
	{
		switch (m_sort)
		{
		case TeaSort::Green:
			return "Green Tea";
		case TeaSort::Black:
			return "Black Tea";
		case TeaSort::Herbal:
			return "Herbal Tea";
		default:
			return "Tea";
		}
	}

	double GetCost() const override
	{
		return 30.0;
	}

private:
	TeaSort m_sort;
};

class Milkshake : public IBeverage
{
public:
	Milkshake(MilkshakeSize size = MilkshakeSize::Medium)
		: m_size(size)
	{
	}

	std::string GetDescription() const override
	{
		switch (m_size)
		{
		case MilkshakeSize::Small:
			return "Small Milkshake";
		case MilkshakeSize::Medium:
			return "Medium Milkshake";
		case MilkshakeSize::Large:
			return "Large Milkshake";
		default:
			return "Milkshake";
		}
	}

	double GetCost() const override
	{
		switch (m_size)
		{
		case MilkshakeSize::Small:
			return 50.0;
		case MilkshakeSize::Medium:
			return 60.0;
		case MilkshakeSize::Large:
			return 80.0;
		}
		return 0.0;
	}

private:
	MilkshakeSize m_size;
};
