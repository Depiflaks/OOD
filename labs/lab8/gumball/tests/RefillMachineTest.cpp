//
// Created by smmm on 03.12.2025.
//
#include "CoutMock.h"
#include "lib/GumBallMachine.h"

TEST_F(GumballMachineTest, RefillFromEmptyMachineChangesToNoQuarter)
{
	CaptureOutput();
	GumballMachine machine(0);

	machine.RefillMachine(3);

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, RefillFromNonEmptyNoQuarterStaysNoQuarter)
{
	CaptureOutput();
	GumballMachine machine(1);

	machine.RefillMachine(3);

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 4 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, RefillWithHasQuartersPreservesHasQuarters)
{
	CaptureOutput();
	GumballMachine machine(1);

	machine.InsertQuarter();
	machine.RefillMachine(3);

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 4 gumballs
Coins: 1/5 quarter(s)
Machine is waiting for turn of crank
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, RefillWithHasQuartersPreservesHasQuartersFromSoldOut)
{
	CaptureOutput();
	GumballMachine machine(1);

	machine.InsertQuarter();
	machine.TurnCrank();

	machine.RefillMachine(3);

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, RefillFullQuartersThenFiveVendsLeadsToSoldOut)
{
	CaptureOutput();
	GumballMachine machine(1);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.RefillMachine(4);

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 0 gumballs
Coins: 0/5 quarter(s)
Machine is sold out
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}
