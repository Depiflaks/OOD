#include "../src/lib/GumBallMachine.h"
#include "CoutMock.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

TEST_F(GumballMachineTest, TwoBalls_InsertQuarter_TurnCrank_CheckState)
{
	CaptureOutput();
	GumballMachine machine(2);

	machine.InsertQuarter();
	machine.TurnCrank();

	ReleaseOutput();
	const auto out = GetOutput();

	EXPECT_EQ(out,
		"You inserted a quarter\n"
		"Pull the lever, Kronk!\n"
		"A gumball comes rolling out the slot...\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 1 gumball
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, OneBall_InsertQuarter_TurnCrank_CheckSoldOutState)
{
	CaptureOutput();
	GumballMachine machine(1);

	machine.InsertQuarter();
	machine.TurnCrank();

	ReleaseOutput();
	const auto out = GetOutput();

	EXPECT_EQ(out,
		"You inserted a quarter\n"
		"Pull the lever, Kronk!\n"
		"A gumball comes rolling out the slot...\n"
		"Oops, out of gumballs\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 0 gumballs
Coins: 0/5 quarter(s)
Machine is sold out
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, TwoBalls_InsertQuarter_Eject_TurnCrank_CheckState)
{
	CaptureOutput();
	GumballMachine machine(2);

	machine.InsertQuarter();
	machine.EjectQuarter();
	machine.TurnCrank();

	ReleaseOutput();
	const auto out = GetOutput();

	EXPECT_EQ(out,
		"You inserted a quarter\n"
		"1 quarter(s) returned\n"
		"All quarters returned\n"
		"You turned but there's no quarters\n"
		"You need to pay first\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 2 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, ZeroBalls_InsertQuarter_CheckState)
{
	CaptureOutput();
	GumballMachine machine(0);

	machine.InsertQuarter();

	ReleaseOutput();
	const auto out = GetOutput();

	EXPECT_EQ(out, "You can't insert a quarter, the machine is sold out\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 0 gumballs
Coins: 0/5 quarter(s)
Machine is sold out
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, ThreeBalls_TurnCrank_EjectQuarter_CheckState)
{
	CaptureOutput();
	GumballMachine machine(3);

	machine.TurnCrank();
	machine.EjectQuarter();

	ReleaseOutput();
	const auto out = GetOutput();

	EXPECT_EQ(out,
		"You turned but there's no quarters\n"
		"You need to pay first\n"
		"You haven't inserted any quarters\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}
