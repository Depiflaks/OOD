#include "../src/lib/Beverages.h"
#include "../src/lib/Condiments.h"
#include "../src/lib/IBeverage.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

template <typename Condiment, typename... Args>
auto MakeCondiment(const Args&... args)
{
	return [=](auto&& b) {
		return std::make_unique<Condiment>(std::forward<decltype(b)>(b), args...);
	};
}

template <typename Component, typename Decorator>
auto operator<<(Component&& component, const Decorator& decorate)
{
	return decorate(std::forward<Component>(component));
}

class MockBeverage : public IBeverage
{
public:
	MOCK_CONST_METHOD0(GetDescription, std::string());
	MOCK_CONST_METHOD0(GetCost, double());
};

TEST(DecoratorInterface, MockBeverageWithoutDecorators)
{
	using ::testing::Return;

	MockBeverage beverage;
	EXPECT_CALL(beverage, GetDescription()).WillOnce(Return("Mock Description"));
	EXPECT_CALL(beverage, GetCost()).WillOnce(Return(100.0));

	EXPECT_EQ(beverage.GetDescription(), "Mock Description");
	EXPECT_DOUBLE_EQ(beverage.GetCost(), 100.0);
}

TEST(DecoratorInterface, DecoratedMockBeverageWithThreeDecorators)
{
	using ::testing::Return;

	auto mockBase = std::make_unique<MockBeverage>();
	EXPECT_CALL(*mockBase, GetDescription()).WillOnce(Return("Mock Base"));
	EXPECT_CALL(*mockBase, GetCost()).WillOnce(Return(100.0));

	auto beverage = std::move(mockBase)
		<< MakeCondiment<Cinnamon>()
		<< MakeCondiment<Lemon>(2)
		<< MakeCondiment<Cream>();

	std::string expectedDescription = "Mock Base, Cinnamon, Lemon x 2, Cream";
	double expectedCost = 100.0 + 20.0 + (10.0 * 2) + 25.0;

	EXPECT_EQ(beverage->GetDescription(), expectedDescription);
	EXPECT_DOUBLE_EQ(beverage->GetCost(), expectedCost);
}

TEST(BeverageTests, PlainCoffee)
{
	auto beverage = std::make_unique<Coffee>();
	EXPECT_EQ(beverage->GetDescription(), "Coffee");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 50.0);
}

TEST(BeverageTests, StandardCappuccino)
{
	auto beverage = std::make_unique<Cappuccino>(CoffeePortion::Standard);
	EXPECT_EQ(beverage->GetDescription(), "Double Cappuccino");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 80.0);
}

TEST(BeverageTests, DoubleCappuccino)
{
	auto beverage = std::make_unique<Cappuccino>(CoffeePortion::Double);
	EXPECT_EQ(beverage->GetDescription(), "Double Cappuccino");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 120.0);
}

TEST(BeverageTests, StandardLatte)
{
	auto beverage = std::make_unique<Latte>(CoffeePortion::Standard);
	EXPECT_EQ(beverage->GetDescription(), "Double Latte");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 90.0);
}

TEST(BeverageTests, DoubleLatte)
{
	auto beverage = std::make_unique<Latte>(CoffeePortion::Double);
	EXPECT_EQ(beverage->GetDescription(), "Double Latte");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 130.0);
}

TEST(BeverageTests, BlackTea)
{
	auto beverage = std::make_unique<Tea>(TeaSort::Black);
	EXPECT_EQ(beverage->GetDescription(), "Black Tea");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 30.0);
}

TEST(BeverageTests, GreenTea)
{
	auto beverage = std::make_unique<Tea>(TeaSort::Green);
	EXPECT_EQ(beverage->GetDescription(), "Green Tea");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 30.0);
}

TEST(BeverageTests, HerbalTea)
{
	auto beverage = std::make_unique<Tea>(TeaSort::Herbal);
	EXPECT_EQ(beverage->GetDescription(), "Herbal Tea");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 30.0);
}

TEST(BeverageTests, SmallMilkshake)
{
	auto beverage = std::make_unique<Milkshake>(MilkshakeSize::Small);
	EXPECT_EQ(beverage->GetDescription(), "Small Milkshake");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 50.0);
}

TEST(BeverageTests, MediumMilkshake)
{
	auto beverage = std::make_unique<Milkshake>(MilkshakeSize::Medium);
	EXPECT_EQ(beverage->GetDescription(), "Medium Milkshake");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 60.0);
}

TEST(BeverageTests, LargeMilkshake)
{
	auto beverage = std::make_unique<Milkshake>(MilkshakeSize::Large);
	EXPECT_EQ(beverage->GetDescription(), "Large Milkshake");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 80.0);
}

TEST(BeverageTests, BeverageWithOneDecorator)
{
	auto beverage = std::make_unique<Latte>(CoffeePortion::Standard)
		<< MakeCondiment<Syrup>(SyrupType::Maple);

	EXPECT_EQ(beverage->GetDescription(), "Double Latte, Maple syrup");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 90.0 + 15.0);
}

TEST(BeverageTests, BeverageWithMultipleSameDecorators)
{
	auto beverage = std::make_unique<Tea>(TeaSort::Green)
		<< MakeCondiment<Lemon>(2)
		<< MakeCondiment<Lemon>(3);

	EXPECT_EQ(beverage->GetDescription(), "Green Tea, Lemon x 2, Lemon x 3");
	EXPECT_DOUBLE_EQ(beverage->GetCost(), 30.0 + (10.0 * 2) + (10.0 * 3));
}

TEST(BeverageTests, HeavilyDecoratedBeverage)
{
	auto beverage = std::make_unique<Milkshake>(MilkshakeSize::Large)
		<< MakeCondiment<Cinnamon>()
		<< MakeCondiment<Lemon>(2)
		<< MakeCondiment<IceCubes>(3, IceCubeType::Water)
		<< MakeCondiment<IceCubes>(1, IceCubeType::Dry)
		<< MakeCondiment<Syrup>(SyrupType::Chocolate)
		<< MakeCondiment<Syrup>(SyrupType::Maple)
		<< MakeCondiment<ChocolateCrumbs>(10)
		<< MakeCondiment<ChocolateCrumbs>(5)
		<< MakeCondiment<CoconutFlakes>(15)
		<< MakeCondiment<CoconutFlakes>(20)
		<< MakeCondiment<Cream>()
		<< MakeCondiment<Cream>()
		<< MakeCondiment<Chocolate>(1)
		<< MakeCondiment<Chocolate>(4)
		<< MakeCondiment<Liquor>(LiquorType::Nut)
		<< MakeCondiment<Liquor>(LiquorType::Chocolate)
		<< MakeCondiment<Cinnamon>()
		<< MakeCondiment<Lemon>(1)
		<< MakeCondiment<Chocolate>(5)
		<< MakeCondiment<Cream>();

	std::string expectedDescription = "Large Milkshake, "
									  "Cinnamon, "
									  "Lemon x 2, "
									  "Water ice cubes x 3, "
									  "Dry ice cubes x 1, "
									  "Chocolate syrup, "
									  "Maple syrup, "
									  "Chocolate crumbs 10g, "
									  "Chocolate crumbs 5g, "
									  "Coconut flakes 15g, "
									  "Coconut flakes 20g, "
									  "Cream, "
									  "Cream, "
									  "Chocolate x 1, "
									  "Chocolate x 4, "
									  "Nut Liquor, "
									  "Chocolate Liquor, "
									  "Cinnamon, "
									  "Lemon x 1, "
									  "Chocolate x 5, "
									  "Cream";

	double expectedCost = 80.0
		+ 20.0 + 20.0 + 15.0 + 10.0 + 15.0 + 15.0 + 20.0
		+ 10.0 + 15.0 + 20.0 + 25.0 + 25.0 + 10.0 + 40.0
		+ 50.0 + 50.0 + 20.0 + 10.0 + 50.0 + 25.0;

	EXPECT_EQ(beverage->GetDescription(), expectedDescription);
	EXPECT_DOUBLE_EQ(beverage->GetCost(), expectedCost);
}
