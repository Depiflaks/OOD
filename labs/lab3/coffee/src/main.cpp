#include "./lib/Condiments.h"
#include "./lib/Beverages.h"
#include "./lib/MakeCondiment.h"

#include <functional>
#include <iostream>

using namespace std;

/*
Функциональный объект, создающий лимонную добавку
*/
struct MakeLemon
{
	MakeLemon(unsigned quantity)
		: m_quantity(quantity)
	{
	}

	auto operator()(IBeveragePtr&& beverage) const
	{
		return std::make_unique<Lemon>(std::move(beverage), m_quantity);
	}

private:
	unsigned m_quantity;
};

/*
Функция, возвращающая функцию, создающую коричную добавку
*/
function<IBeveragePtr(IBeveragePtr&&)> MakeCinnamon()
{
	return [](IBeveragePtr&& b) {
		return std::make_unique<Cinnamon>(std::move(b));
	};
}

/*
Перегруженная версия оператора <<, которая предоставляет нам синтаксический сахар
для декорирования компонента

Позволяет создать цепочку оборачивающих напиток декораторов следующим образом:
auto beverage = std::make_unique<CConcreteBeverage>(a, b, c)
					<< MakeCondimentA(d, e, f)
					<< MakeCondimentB(g, h);

Функциональные объекты MakeCondiment* запоминают аргументы, необходимые для создания
дополнения, и возвращают фабричную функцию, принимающую оборачиваемый напиток, которая
при своем вызове создаст нужный объект Condiment, передав ему запомненные аргументы.
Использование:
	auto beverage =
		std::make_unique<CConcreteBeverage>(a, b, c)
		<< MakeCondimentA(d, e, f)
		<< MakeCondimentB(g, h);
или даже так:
	auto beverage =
		std::make_unique<CConcreteBeverage>
		<< MakeCondiment<CondimentA>(d, e, f)
		<< MakeCondiment<CondimentB>(g, h);
В последнем случае нет необходимости писать вручную реализации MakeCondimentA и MakeCondimentB, т.к.
необходимую реализацию сгенерирует компилятор

Классический способ оборачивания выглядел бы так:
	auto baseBeverage = std::make_unique<CConcretedBeverage>(a, b, c);
	auto wrappedWithCondimentA = std::make_unique<CCondimentA>(std::move(baseBeverage), d, e, f);
	auto beverage = std::make_unique<CCondimentB>(std::move(wrappedWithCondimentA), g, h);
либо так:
	auto beverage = std::make_unique<CCondimentB>(
						std::make_unique<CCondimentA>(
							std::make_unique<CConcreteBeverage>(a, b, c), // Напиток
							d, e, f	// доп. параметы CondimentA
						),
						g, h		// доп. параметры CondimentB
					);

unique_ptr<Lemon> operator << (IBeveragePtr && lhs, const MakeLemon & factory)
{
	return factory(std::move(lhs));
}
unique_ptr<Cinnamon> operator << (IBeveragePtr && lhs, const MakeCinnamon & factory)
{
	return factory(std::move(lhs));
}
*/

void DialogWithUser()
{
	cout << "Type 1 for coffee or 2 for tea\n";
	int beverageChoice;
	cin >> beverageChoice;

	unique_ptr<IBeverage> beverage;

	if (beverageChoice == 1)
	{
		beverage = std::make_unique<Coffee>();
	}
	else if (beverageChoice == 2)
	{
		beverage = std::make_unique<Tea>();
	}
	else
	{
		return;
	}

	int condimentChoice;
	for (;;)
	{
		cout << "1 - Lemon, 2 - Cinnamon, 0 - Checkout" << endl;
		cin >> condimentChoice;

		if (condimentChoice == 1)
		{
			// beverage = std::make_unique<Lemon>(std::move(beverage));
			beverage = std::move(beverage) << MakeCondiment<Lemon>(2);
		}
		else if (condimentChoice == 2)
		{
			// beverage = std::make_unique<Cinnamon>(std::move(beverage));
			beverage = std::move(beverage) << MakeCondiment<Cinnamon>();
		}
		else if (condimentChoice == 0)
		{
			break;
		}
		else
		{
			return;
		}
	}

	cout << beverage->GetDescription() << ", cost: " << beverage->GetCost() << endl;
}

int main()
{
	DialogWithUser();
	cout << endl;
	{
		// Наливаем чашечку латте
		auto latte = std::make_unique<Latte>();
		// добавляем корицы
		auto cinnamon = std::make_unique<Cinnamon>(std::move(latte));
		// добавляем пару долек лимона
		auto lemon = std::make_unique<Lemon>(std::move(cinnamon), 2);
		// добавляем пару кубиков льда
		auto iceCubes = std::make_unique<IceCubes>(std::move(lemon), 2, IceCubeType::Dry);
		// добавляем 2 грамма шоколадной крошки
		auto beverage = std::make_unique<ChocolateCrumbs>(std::move(iceCubes), 2);

		// Выписываем счет покупателю
		cout << beverage->GetDescription() << " costs " << beverage->GetCost() << endl;
	}

	{
		auto beverage = std::make_unique<ChocolateCrumbs>( // Внешний слой: шоколадная крошка
			std::make_unique<IceCubes>( // | под нею - кубики льда
				std::make_unique<Lemon>( // | | еще ниже лимон
					std::make_unique<Cinnamon>( // | | | слоем ниже - корица
						std::make_unique<Latte>()), // | | |   в самом сердце - Латте
					2), // | | 2 дольки лимона
				2, IceCubeType::Dry), // | 2 кубика сухого льда
			2); // 2 грамма шоколадной крошки

		// Выписываем счет покупателю
		cout << beverage->GetDescription() << " costs " << beverage->GetCost() << endl;
	}

	// Подробнее рассмотрим работу MakeCondiment и оператора <<
	{
		// lemon - функция, добавляющая "2 дольки лимона" к любому напитку
		auto lemon2 = MakeCondiment<Lemon>(2);
		// iceCubes - функция, добавляющая "3 кусочка льда" к любому напитку
		auto iceCubes3 = MakeCondiment<IceCubes>(3, IceCubeType::Water);

		auto tea = std::make_unique<Tea>();

		// декорируем чай двумя дольками лимона и тремя кусочками льда
		auto lemonIceTea = iceCubes3(lemon2(std::move(tea)));

		auto oneMoreLemonIceTea = std::make_unique<Tea>() // Берем чай
			<< MakeCondiment<Lemon>(2) // добавляем пару долек лимона
			<< MakeCondiment<IceCubes>(3, IceCubeType::Water); // и 3 кубика льда
	}

	{
		auto beverage = std::make_unique<Latte>() // Наливаем чашечку латте,
			<< MakeCondiment<Cinnamon>() // оборачиваем корицей,
			<< MakeCondiment<Lemon>(2) // добавляем пару долек лимона
			<< MakeCondiment<IceCubes>(2, IceCubeType::Dry) // брасаем пару кубиков сухого льда
			<< MakeCondiment<ChocolateCrumbs>(2); // посыпаем шоколадной крошкой

		// Выписываем счет покупателю
		cout << beverage->GetDescription() << " costs " << beverage->GetCost() << endl;
	}

	{
		auto beverage = std::make_unique<Milkshake>() // Наливаем молочный коктейль
			<< MakeCondiment<Syrup>(SyrupType::Maple) // заливаем кленовым сиропом
			<< MakeCondiment<CoconutFlakes>(8); // посыпаем кокосовой стружкой

		// Выписываем счет покупателю
		cout << beverage->GetDescription() << " costs " << beverage->GetCost() << endl;
	}
}
