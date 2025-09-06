#include "../src/lib/Duck/Dance/IDanceBehavior.h"
#include "../src/lib/Duck/Duck.h"
#include "../src/lib/Duck/Fly/FlyWithWings.h"
#include "../src/lib/Duck/Quack/QuackBehavior.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class MockDanceBehavior : public IDanceBehavior
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
	std::unique_ptr<MockDanceBehavior> danceBehavior = std::make_unique<MockDanceBehavior>();

	EXPECT_CALL(*danceBehavior, Dance()).Times(1);

	TestDuck duck(std::move(danceBehavior));
	duck.Dance();
}

