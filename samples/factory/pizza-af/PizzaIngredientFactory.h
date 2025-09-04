#pragma once
#include "Clams.h"
#include "Dough.h"
#include "Cheese.h"
#include "Veggies.h"
#include "Pepperoni.h"
#include "Sauce.h"
#include <memory>
#include <vector>
#include <functional>

typedef std::function<std::unique_ptr<IDough>()> DoughFactory;

std::unique_ptr<IDough> ThinCrustDoughFactory()
{
	return std::make_unique<CThinCrustDough>();
}


class IPizzaIngredientFactory
{
public:
	virtual std::unique_ptr<IDough> CreateDough() = 0;
	virtual std::unique_ptr<ISauce> CreateSauce() = 0;
	virtual std::unique_ptr<ICheese> CreateCheese() = 0;
	virtual std::vector<std::unique_ptr<IVeggies>> CreateVeggies() = 0;
	virtual std::unique_ptr<IPepperoni> CreatePepperoni() = 0;
	virtual std::unique_ptr<IClams> CreateClam() = 0;

	virtual ~IPizzaIngredientFactory() = default;
};

class CNYPizzaIngredientFactory : public IPizzaIngredientFactory
{
public:
	std::unique_ptr<IDough> CreateDough() override
	{
		return std::make_unique<CThinCrustDough>();
	}

	std::unique_ptr<ISauce> CreateSauce() override
	{
		return std::make_unique<CMarinaraSauce>();
	}

	std::unique_ptr<ICheese> CreateCheese() override
	{
		return std::make_unique<CReggianoCheese>();
	}

	std::vector<std::unique_ptr<IVeggies>> CreateVeggies() override
	{
		using namespace std;
		typedef unique_ptr<IVeggies> IVeggiesPtr;

		vector<IVeggiesPtr > veggies;
		veggies.push_back(IVeggiesPtr(new CGarlic()));
		veggies.push_back(IVeggiesPtr(new COnion()));
		veggies.push_back(IVeggiesPtr(new CMushroom()));
		veggies.push_back(IVeggiesPtr(new CRedPepper()));

		return veggies;
	}

	std::unique_ptr<IPepperoni> CreatePepperoni() override
	{
		return std::make_unique<CSlicedPepperoni>();
	}

	std::unique_ptr<IClams> CreateClam() override
	{
		return std::make_unique<CFreshClams>();
	}
};

class CChicagoPizzaIngredientFactory : public IPizzaIngredientFactory
{
public:
	std::unique_ptr<IDough> CreateDough() override
	{
		return std::make_unique<CThickCrustDough>();
	}

	std::unique_ptr<ISauce> CreateSauce() override
	{
		return std::make_unique<CPlumTomatoSauce>();
	}
	
	std::unique_ptr<ICheese> CreateCheese() override
	{
		return std::make_unique<CMozarellaCheese>();
	}
	
	std::vector<std::unique_ptr<IVeggies>> CreateVeggies() override
	{
		using namespace std;
		typedef unique_ptr<IVeggies> IVeggiesPtr;

		vector<IVeggiesPtr > veggies;
		veggies.push_back(IVeggiesPtr(new CBlackOlives()));
		veggies.push_back(IVeggiesPtr(new CSpinach()));
		veggies.push_back(IVeggiesPtr(new CEggplant()));

		return veggies;
	}
	
	std::unique_ptr<IPepperoni> CreatePepperoni() override
	{
		return std::make_unique<CSlicedPepperoni>();
	}
	
	std::unique_ptr<IClams> CreateClam() override
	{
		return std::make_unique<CFrozenClams>();
	}
};
