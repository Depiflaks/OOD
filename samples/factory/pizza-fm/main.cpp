#include <memory>
#include <string>
#include <stdexcept>

#include "Pizza.h"

using namespace std;

class CSimplePizzaFactory
{
public:
	virtual unique_ptr<CPizza> CreatePizza(const string& type) const
	{
		unique_ptr<CPizza> pizza;

		if (type == "cheeze")
		{
			pizza = make_unique<CCheezePizza>();
		}
		else if (type == "clam")
		{
			pizza = make_unique<CClamPizza>();
		}
		else if (type == "peperoni")
		{
			pizza = make_unique<CPeperoniPizza>();
		}
		else if (type == "veggie")
		{
			pizza = make_unique<CVeggiePizza>();
		}
		else
		{
			throw invalid_argument("Unknown pizza type");
		}
		return pizza;
	}

	virtual ~CSimplePizzaFactory() = default;
};

// Пиццерия
class CPizzaStore
{
public:
	// Параметризуем пиццерию объектом фабрики
	CPizzaStore(unique_ptr<CSimplePizzaFactory> && factory)
		:m_factory(move(factory))
	{
	}

	unique_ptr<CPizza> OrderPizza(const string& type)
	{
		// Делегируем создание экземпляра пиццы фабрике
		auto pizza = m_factory->CreatePizza(type);

		pizza->Prepare();
		pizza->Bake();
		pizza->Cut();
		pizza->Box();

		return pizza;
	}
private:
	unique_ptr<CSimplePizzaFactory> m_factory;
};

class CNYPizzaFactory : public CSimplePizzaFactory
{
public:
	unique_ptr<CPizza> CreatePizza(const string& type) const override
	{
		unique_ptr<CPizza> pizza;

		if (type == "cheeze")
		{
			pizza = make_unique<CNYStyleCheezePizza>();
		}
		else if (type == "clam")
		{
			pizza = make_unique<CNYStyleClamPizza>();
		}
		else if (type == "peperoni")
		{
			pizza = make_unique<CNYStylePeperoniPizza>();
		}
		else if (type == "veggie")
		{
			pizza = make_unique<CNYStyleVeggiePizza>();
		}
		else
		{
			throw invalid_argument("Unknown pizza type");
		}
		return pizza;
	}
};

void OrderPizzaWithSimpleFactory()
{
	auto nyPizzaFactory = make_unique<CNYPizzaFactory>();
	CPizzaStore pizzaStore(move(nyPizzaFactory));
	auto pizza = pizzaStore.OrderPizza("peperoni");
	cout << pizza->ToString() << endl;
}

class CAbstractPizzaStore
{
public:
	unique_ptr<CPizza> OrderPizza(const string& type)
	{
		// Делегируем создание экземпляра пиццы подклассам
		auto pizza = CreatePizza(type);

		// Дальнейшие шаги выполняем по строго заданному алгоритму
		pizza->Prepare();
		pizza->Bake();
		pizza->Cut();
		pizza->Box();

		return pizza;
	}
	virtual ~CAbstractPizzaStore() = default;
protected:
	// Конкретные подклассы обязаны реализовать данный метод
	virtual unique_ptr<CPizza> CreatePizza(const string& type) const = 0;
};

// Пиццерия, готовящая пиццу в нью-йоркском стиле
class CNYPizzaStore : public CAbstractPizzaStore
{
protected:
	// Все, что нужно - реализовать метод CreatePizza нужным образом
	unique_ptr<CPizza> CreatePizza(const string& type) const override
	{
		unique_ptr<CPizza> pizza;

		if (type == "cheeze")
		{
			pizza = make_unique<CNYStyleCheezePizza>();
		}
		else if (type == "clam")
		{
			pizza = make_unique<CNYStyleClamPizza>();
		}
		else if (type == "peperoni")
		{
			pizza = make_unique<CNYStylePeperoniPizza>();
		}
		else if (type == "veggie")
		{
			pizza = make_unique<CNYStyleVeggiePizza>();
		}
		else
		{
			throw invalid_argument("Unknown pizza type");
		}
		return pizza;
	}
};

class CChicagoPizzaStore : public CAbstractPizzaStore
{
protected:
	// Все, что нужно - реализовать метод CreatePizza нужным образом
	unique_ptr<CPizza> CreatePizza(const string& type) const override
	{
		unique_ptr<CPizza> pizza;

		if (type == "cheeze")
		{
			pizza = make_unique<CChicagoStyleCheezePizza>();
		}
		else if (type == "clam")
		{
			pizza = make_unique<CChicagoStyleClamPizza>();
		}
		else if (type == "peperoni")
		{
			pizza = make_unique<CChicagoStylePeperoniPizza>();
		}
		else if (type == "veggie")
		{
			pizza = make_unique<CChicagoStyleVeggiePizza>();
		}
		else
		{
			throw invalid_argument("Unknown pizza type");
		}
		return pizza;
	}
};

class CDependentPizzaStore 
{
public:
	unique_ptr<CPizza> OrderPizza(const string & style, const string & type)
	{
		unique_ptr<CPizza> pizza;
		if (style == "NY")
		{
			if (type == "cheeze")
			{
				pizza = make_unique<CNYStyleCheezePizza>();
			}
			else if (type == "clam")
			{
				pizza = make_unique<CNYStyleClamPizza>();
			}
			else if (type == "peperoni")
			{
				pizza = make_unique<CNYStylePeperoniPizza>();
			}
			else if (type == "veggie")
			{
				pizza = make_unique<CNYStyleVeggiePizza>();
			}
		}
		else if (style == "Chicago")
		{
			if (type == "cheeze")
			{
				pizza = make_unique<CChicagoStyleCheezePizza>();
			}
			else if (type == "clam")
			{
				pizza = make_unique<CChicagoStyleClamPizza>();
			}
			else if (type == "peperoni")
			{
				pizza = make_unique<CChicagoStylePeperoniPizza>();
			}
			else if (type == "veggie")
			{
				pizza = make_unique<CChicagoStyleVeggiePizza>();
			}
		}
		if (!pizza)
		{
			throw invalid_argument("Unknown pizza type");
		}
		pizza->Prepare();
		pizza->Bake();
		pizza->Cut();
		pizza->Box();
		return pizza;
	}
};


void TestDrivePizzaStore(CAbstractPizzaStore & store)
{
	auto pizza = store.OrderPizza("cheeze");
	
	cout << "I have ordered a " << pizza->GetName() << endl << endl;
}

void OrderPizzaWithFactoryMethod()
{
	CNYPizzaStore nyStore;
	TestDrivePizzaStore(nyStore);

	CChicagoPizzaStore chicagoStore;
	TestDrivePizzaStore(chicagoStore);
}

int main()
{
	cout << "Ordering pizza with Simple Factory" << endl << endl;
	OrderPizzaWithSimpleFactory();

	cout << "-------------------" << endl;

	cout << "Ordering pizza using Factory Method pattern" << endl << endl;
	OrderPizzaWithFactoryMethod();

	cout << "-------------------" << endl;

	return 0;
}
