#pragma once

#include "IBeverage.h"

class Beverage : public IBeverage
{
public:
	Beverage(const std::string& description)
		: m_description(description)
	{
	}

	std::string GetDescription() const override final
	{
		return m_description;
	}

private:
	std::string m_description;
};

// Кофе
class Coffee : public Beverage
{
public:
	Coffee(const std::string& description = "Coffee")
		: Beverage(description)
	{
	}

	double GetCost() const override
	{
		return 60;
	}
};

// Капуччино
class Cappuccino : public Coffee
{
public:
	Cappuccino()
		: Coffee("Cappuccino")
	{
	}

	double GetCost() const override
	{
		return 80;
	}
};

// Латте
class Latte : public Coffee
{
public:
	Latte()
		: Coffee("Latte")
	{
	}

	double GetCost() const override
	{
		return 90;
	}
};

// Чай
class Tea : public Beverage
{
public:
	Tea()
		: Beverage("Tea")
	{
	}

	double GetCost() const override
	{
		return 30;
	}
};

// Молочный коктейль
class Milkshake : public Beverage
{
public:
	Milkshake()
		: Beverage("Milkshake")
	{
	}

	double GetCost() const override
	{
		return 80;
	}
};
