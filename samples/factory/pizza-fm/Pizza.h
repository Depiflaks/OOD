#pragma once
#include <iostream>
#include <string>
#include <vector>

// Базовый класс Пиццы
class CPizza
{
public:
	typedef std::vector<std::string> Toppings;
	CPizza(const std::string & name, 
		const std::string & dough, 
		const std::string & sauce)
		: m_name(name), m_dough(dough)
		, m_sauce(sauce)
	{}

	void Prepare()
	{
		std::cout << "Preparing " << m_name << std::endl;
		std::cout << "Tossing dough..." << std::endl;
		std::cout << "Adding sauce..." << std::endl;
		std::cout << "Adding toppings:" << std::endl;
		for (auto & topping : m_toppings)
		{
			std::cout << "    " + topping << std::endl;
		}
	}

	void Bake()
	{
		std::cout << "Bake for 25 minutes at 350''\n";
	}

	virtual void Cut()
	{
		std::cout << "Cutting the pizza into diagonal slices\n";
	}

	void Box()
	{
		std::cout << "Place pizza in official PizzaStore box\n";
	}

	std::string GetName()const
	{
		return m_name;
	}

	std::string ToString()const
	{
		std::string descr = "---- " + m_name + " ----\n";
		descr += m_dough + "\n";
		descr += m_sauce + "\n";
		for (auto & topping : m_toppings)
		{
			descr += topping + "\n";
		}
		return descr;
	}

	virtual ~CPizza() = default;
protected:
	void AddTopping(const std::string& topping)
	{
		m_toppings.push_back(topping);
	}
private:
	std::string m_name;
	std::string m_dough;
	std::string m_sauce;
	std::vector<std::string> m_toppings;
};

class CCheezePizza : public CPizza
{
public:
	CCheezePizza() : CPizza(
		"Cheese Pizza",
		"Regular Crust",
		"Marinara Pizza Sauce")
	{
		AddTopping("Fresh Mozzarella");
		AddTopping("Parmesan");
	}
};

class CClamPizza : public CPizza
{
public:
	CClamPizza() : CPizza(
		"Clam Pizza",
		"Thin crust",
		"White garlic sauce")
	{
		AddTopping("Clams");
		AddTopping("Grated parmesan cheese");
	}
};

class CPeperoniPizza : public CPizza 
{
public:
	CPeperoniPizza() : CPizza(
		"Peperoni Pizza",
		"Crust",
		"Marinara sauce")
	{
		AddTopping("Sliced Peperoni");
		AddTopping("Sliced Onion");
		AddTopping("Grated parmesan cheese");
	}
};

class CVeggiePizza : public CPizza 
{
public:
	CVeggiePizza() : CPizza(
		"Veggie Pizza",
		"Crust",
		"Marinara sauce")
	{
		AddTopping("Shredded mozzarella");
		AddTopping("Grated parmesan");
		AddTopping("Diced onion");
		AddTopping("Sliced mushrooms");
		AddTopping("Sliced red pepper");
		AddTopping("Sliced black olives");
	}
};

//////////////
class CNYStyleCheezePizza : public CPizza
{
public:
	CNYStyleCheezePizza() : CPizza(
		"NY Style Sauce and Cheese Pizza", 
		"Thin Crust Dough", 
		"Marinara Sauce")
	{
		AddTopping("Grated Reggiano Cheese");
	}
};

class CNYStyleClamPizza : public CPizza
{
public:
	CNYStyleClamPizza() :CPizza(
		"NY Style Clam Pizza",
		"Thin Crust Dough",
		"Marinara Sauce") 
	{
		AddTopping("Grated Reggiano Cheese");
		AddTopping("Fresh Clams from Long Island Sound");
	}
};

class CNYStylePeperoniPizza : public CPizza
{
public:
	CNYStylePeperoniPizza() : CPizza(
		"NY Style Peperoni Pizza",
		"Thin Crust Dough",
		"Marinara Sauce")
	{
		AddTopping("Grated Reggiano Cheese");
		AddTopping("Sliced Peperoni");
		AddTopping("Garlic");
		AddTopping("Onion");
		AddTopping("Mushrooms");
		AddTopping("Red Pepper");
	}
};

class CNYStyleVeggiePizza : public CPizza
{
public:
	CNYStyleVeggiePizza() : CPizza(
		"NY Style Veggie Pizza",
		"Thin Crust Dough",
		"Marinara Sauce")
	{
		AddTopping("Grated Reggiano Cheese");
		AddTopping("Garlic");
		AddTopping("Onion");
		AddTopping("Mushrooms");
		AddTopping("Red Pepper");
	}
};

///
class CChicagoStyleCheezePizza : public CPizza
{
public:
	CChicagoStyleCheezePizza() : CPizza(
		"Chicago Style Deep Dish Cheese Pizza",
		"Extra Thick Crust Dough",
		"Plum Tomato Sauce")
	{
		AddTopping("Shredded Mozzarella Cheese");
	}
};

class CChicagoStyleClamPizza : public CPizza
{
public:
	CChicagoStyleClamPizza() :CPizza(
		"Chicago Style Clam Pizza",
		"Extra Thick Crust Dough",
		"Plum Tomato Sauce")
	{
		AddTopping("Shredded Mozzarella Cheese");
		AddTopping("Frozen Clams from Chesapeake Bay");
	}

	void Cut()override
	{
		std::cout << "Cutting the pizza into square slices" << std::endl;
	}
};

class CChicagoStylePeperoniPizza : public CPizza
{
public:
	CChicagoStylePeperoniPizza() : CPizza(
		"Chicago Style Peperoni Pizza",
		"Extra Thick Crust Dough",
		"Plum Tomato Sauce")
	{
		AddTopping("Shredded Mozzarella Cheese");
		AddTopping("Black Olives");
		AddTopping("Spinach");
		AddTopping("Eggplant");
		AddTopping("Sliced Peperoni");
	}

	void Cut()override
	{
		std::cout << "Cutting the pizza into square slices" << std::endl;
	}
};

class CChicagoStyleVeggiePizza : public CPizza
{
public:
	CChicagoStyleVeggiePizza() : CPizza(
		"Chicago Deep Dish Veggie Pizza",
		"Extra Thick Crust Dough",
		"Plum Tomato Sauce")
	{
		AddTopping("Shredded Mozzarella Cheese");
		AddTopping("Black Olives");
		AddTopping("Spinach");
		AddTopping("Eggplant");
	}
};