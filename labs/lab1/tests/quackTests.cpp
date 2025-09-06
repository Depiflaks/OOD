#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../src/lib/Duck/Dance/NoDanceBehavior.h"
#include "../src/lib/Duck/Duck.h"
#include "../src/lib/Duck/Fly/FlyNoWay.h"
#include "../src/lib/Duck/Fly/FlyWithWings.h"
#include "../src/lib/Duck/Quack/QuackBehavior.h"

class TestDuck : public Duck
{
public:
	TestDuck(
		std::unique_ptr<IFlyBehavior>&& flyBehavior,
		std::unique_ptr<IQuackBehavior>&& quackBehavior,
		std::unique_ptr<IDanceBehavior>&& danceBehavior)
		: Duck(std::move(flyBehavior),
			  std::move(quackBehavior),
			  std::move(danceBehavior))
	{
	}

	MOCK_METHOD(void, Quack, (), ());

	void Display() const override
	{
	}
};

class MockFlyBehavior : public IFlyBehavior
{
public:
	MOCK_METHOD(void, Fly, (), (override));
	MOCK_METHOD(bool, CanFly, (), (const, override));
	MOCK_METHOD(int, GetFlyCount, (), (const, override));
};

class MockQuackBehavior : public IQuackBehavior
{
public:
	MOCK_METHOD(void, Quack, (), (override));
};

class DuckTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mockFlyBehavior = std::make_unique<MockFlyBehavior>();
		mockQuackBehavior = std::make_unique<MockQuackBehavior>();
		rawMockFly = mockFlyBehavior.get();
		rawMockQuack = mockQuackBehavior.get();
		duck = std::make_unique<TestDuck>(
			std::move(mockFlyBehavior),
			std::move(mockQuackBehavior),
			std::make_unique<NoDanceBehavior>());
	}

	std::unique_ptr<TestDuck> duck;
	MockFlyBehavior* rawMockFly;
	MockQuackBehavior* rawMockQuack;

private:
	std::unique_ptr<MockFlyBehavior> mockFlyBehavior;
	std::unique_ptr<MockQuackBehavior> mockQuackBehavior;
};

TEST_F(DuckTest, QuackCalledWhenEvenFlyCount)
{
	EXPECT_CALL(*rawMockFly, Fly()).Times(1);
	EXPECT_CALL(*rawMockFly, CanFly()).WillRepeatedly(testing::Return(true));
	EXPECT_CALL(*rawMockFly, GetFlyCount()).WillRepeatedly(testing::Return(4));
	EXPECT_CALL(*rawMockQuack, Quack()).Times(1);

	duck->Fly();
}

TEST_F(DuckTest, QuackDontCalledWhenOddFlyCount)
{
	EXPECT_CALL(*rawMockFly, Fly()).Times(1);
	EXPECT_CALL(*rawMockFly, CanFly()).WillRepeatedly(testing::Return(true));
	EXPECT_CALL(*rawMockFly, GetFlyCount()).WillRepeatedly(testing::Return(3));
	EXPECT_CALL(*rawMockQuack, Quack()).Times(0);

	duck->Fly();
}

TEST_F(DuckTest, QuackDontCalledWhenEvenFlyCountAndCanNotFly)
{
	EXPECT_CALL(*rawMockFly, Fly()).Times(1);
	EXPECT_CALL(*rawMockFly, CanFly()).WillRepeatedly(testing::Return(false));
	EXPECT_CALL(*rawMockFly, GetFlyCount()).WillRepeatedly(testing::Return(4));
	EXPECT_CALL(*rawMockQuack, Quack()).Times(0);

	duck->Fly();
}

