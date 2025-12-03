//
// Created by smmm on 03.12.2025.
//
#include "CoutMock.h"
#include "lib/GumBallMachine.h"

TEST_F(GumballMachineTest, InsertQuarterFourTimes_HasQuartersState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 4 gumballs
Coins: 4/5 quarter(s)
Machine is waiting for turn of crank
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, InsertQuarterFiveTimes_FullQuartersState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 4 gumballs
Coins: 5/5 quarter(s)
Machine is full of quarters
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, InsertQuarterSixTimes_StillFullQuartersState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 4 gumballs
Coins: 5/5 quarter(s)
Machine is full of quarters
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_ThreeQuarters_ThreeCranks_NoQuarterState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 3/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_ThreeQuarters_FourCranks_NoQuarterState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 3/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_FourQuarters_ThreeCranks_NoQuarterState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 4/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_FourQuarters_FourCranks_FinalState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 4/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_ComplexSequence_TwoVends_FinalState)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();

	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 2 gumballs
Coins: 3/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, FourBalls_ThreeQuarters_ThenEject_AllReturned)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.EjectQuarter();

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

TEST_F(GumballMachineTest, FourBalls_ThreeQuarters_OneCrank_ThenEject)
{
	CaptureOutput();
	GumballMachine machine(4);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.EjectQuarter();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 3/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, ThreeBalls_FourQuarters_FourCranks_ThenEject)
{
	CaptureOutput();
	GumballMachine machine(3);

	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();
	machine.InsertQuarter();

	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();
	machine.TurnCrank();

	machine.EjectQuarter();

	ReleaseOutput();

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 2 gumballs
Coins: 4/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}
