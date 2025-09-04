#pragma once
#include "Cheese.h"
#include "Clams.h"
#include "Dough.h"
#include "Pepperoni.h"
#include "Sauce.h"
#include "Veggies.h"

#include <memory>
#include <vector>
#include <iostream>
#include "PizzaIngredientFactory.h"


class CPizza
{
public:
	virtual void Prepare() = 0;

	void Bake()
	{
		std::cout << "Bake for 25 minutes at 350" << std::endl;
	}

	void Cut()
	{
		std::cout << "Cutting the pizza into diagonal slices" << std::endl;
	}

	void Box() 
	{
		std::cout << "Place pizza in official PizzaStore box" << std::endl;
	}

	std::string GetName()const
	{
		return m_name;
	}

	void SetName(const std::string & name)
	{
		m_name = name;
	}

	std::string ToString()const
	{
		// Код вывода описания пиццы
		std::string result;
		result.append("---- " + m_name + " ----\n");
		if (m_dough)
		{
			result.append(m_dough->ToString() + "\n");
		}
		if (m_sauce)
		{
			result.append(m_sauce->ToString() + "\n");
		}
		if (m_cheese)
		{
			result.append(m_cheese->ToString() + "\n");
		}

		for (auto & v : m_veggies)
		{
			result.append(v->ToString() + ", ");
		}
		if (m_clam)
		{
			result.append(m_clam->ToString() + "\n");
		}
		if (m_pepperoni)
		{
			result.append(m_pepperoni->ToString() + "\n");
		}
		return result;
	}


	virtual ~CPizza() = default;
protected:
	std::string m_name;
	std::unique_ptr<IDough> m_dough;
	std::unique_ptr<ISauce> m_sauce;
	std::vector<std::unique_ptr<IVeggies>> m_veggies;
	std::unique_ptr<ICheese> m_cheese;
	std::unique_ptr<IPepperoni> m_pepperoni;
	std::unique_ptr<IClams> m_clam;
};

class CCheesePizza : public CPizza
{
public:
	CCheesePizza(std::unique_ptr<IPizzaIngredientFactory> && factory)
		: m_ingredientFactory(std::move(factory))
	{
	}

	void Prepare() override
	{
		std::cout << "Preparing " << GetName() << std::endl;

		m_dough = m_ingredientFactory->CreateDough();
		m_sauce = m_ingredientFactory->CreateSauce();
		m_cheese = m_ingredientFactory->CreateCheese();
	}
private:
	std::unique_ptr<IPizzaIngredientFactory> m_ingredientFactory;
};

/* Пицца из мидий - тесто, соус, сыр, мидии*/
class CClamPizza : public CPizza
{
public:
	CClamPizza(std::unique_ptr<IPizzaIngredientFactory> && factory)
		: m_ingredientFactory(std::move(factory))
	{
	}

	void Prepare() override
	{
		std::cout << "Preparing " << GetName() << std::endl;

		m_dough = m_ingredientFactory->CreateDough();
		m_sauce = m_ingredientFactory->CreateSauce();
		m_cheese = m_ingredientFactory->CreateCheese();
		m_clam = m_ingredientFactory->CreateClam();
	}
private:
	std::unique_ptr<IPizzaIngredientFactory> m_ingredientFactory;
};

/*
Лаваш - иллюстрирует использование фабрики в функциональном стиле
Вместо одной большой универсальной фабрики принимает одну мини-фабрику по производству теста
Если понадобится использовать более одного ингредиента, можно передать доп. мини-фабрику
*/
class CLavash : public CPizza
{
public:
	CLavash(const DoughFactory & doughFactory 
		/* при желании можно добавить дополнительные мини-фабрики ингредиентов*/)
		:m_doughFactory(doughFactory)
	{
	}
	void Prepare() override
	{
		std::cout << "Preparing " << GetName() << std::endl;

		m_dough = m_doughFactory();
	}
private:
	DoughFactory m_doughFactory;
};