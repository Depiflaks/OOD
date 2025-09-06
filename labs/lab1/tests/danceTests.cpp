#include "../src/lib/Duck/Dance/IDanceBehavior.h"
#include "../src/lib/Duck/Duck.h"
#include "../src/lib/Duck/Fly/FlyWithWings.h"
#include "../src/lib/Duck/Quack/QuackBehavior.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class MockDanceBehavior1 : public IDanceBehavior
{
public:
	MOCK_METHOD(void, Dance, (), (override));
};

class MockDanceBehavior2 : public IDanceBehavior
{
public:
	MOCK_METHOD(void, Dance, (), (override));
};

class TestDuck : public Duck
{
public:
	explicit TestDuck(std::unique_ptr<IDanceBehavior>&& danceBehavior)
		: Duck(
			  std::make_unique<FlyWithWings>(),
			  std::make_unique<QuackBehavior>(),
			  std::move(danceBehavior))
	{
	}

	void Display() const override
	{
		std::cout << "test duck\n";
	}
};

TEST(DuckDanceTest, TestDanceBehavior)
{
	std::unique_ptr<MockDanceBehavior1> danceBehavior = std::make_unique<MockDanceBehavior1>();

	EXPECT_CALL(*danceBehavior, Dance()).Times(1);

	TestDuck duck(std::move(danceBehavior));
	duck.Dance();
}

TEST(DuckDanceTest, TestDanceBehaviorWhenChangeDanceBehavior)
{
	std::unique_ptr<MockDanceBehavior1> danceBehavior1 = std::make_unique<MockDanceBehavior1>();

	std::unique_ptr<MockDanceBehavior2> danceBehavior2 = std::make_unique<MockDanceBehavior2>();

	EXPECT_CALL(*danceBehavior1, Dance()).Times(1);
	EXPECT_CALL(*danceBehavior2, Dance()).Times(1);

	TestDuck duck(std::move(danceBehavior1));
	duck.Dance();
	duck.SetDanceBehavior(std::move(danceBehavior2));
	duck.Dance();
}
