#include "../src/lib/GumBallMachine.h"
#include "CoutMock.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

TEST_F(GumballMachineTest, SoldOutStateActions)
{
	GumballMachine machine(0);
	CaptureOutput();

	machine.InsertQuarter();
	EXPECT_EQ(
		GetOutput(), "You can't insert a quarter, the machine is sold out\n");
	ClearOutput();

	machine.EjectQuarter();
	EXPECT_EQ(
		GetOutput(), "You can't eject, you haven't inserted a quarter yet\n");
	ClearOutput();

	machine.TurnCrank();
	EXPECT_EQ(GetOutput(),
		"You turned but there's no gumballs\nNo gumball dispensed\n");

	ReleaseOutput();
}

TEST_F(GumballMachineTest, NoQuarterStateActions)
{
	GumballMachine machine(5);
	CaptureOutput();

	machine.EjectQuarter();
	EXPECT_EQ(GetOutput(), "You haven't inserted a quarter\n");
	ClearOutput();

	machine.TurnCrank();
	EXPECT_EQ(GetOutput(),
		"You turned but there's no quarter\nYou need to pay first\n");
	ClearOutput();

	machine.InsertQuarter();
	EXPECT_EQ(GetOutput(), "You inserted a quarter\n");

	ReleaseOutput();
}

TEST_F(GumballMachineTest, HasQuarterStateActions)
{
	GumballMachine machine(5);
	CaptureOutput();

	machine.InsertQuarter();
	ClearOutput();

	machine.InsertQuarter();
	EXPECT_EQ(GetOutput(), "You can't insert another quarter\n");
	ClearOutput();

	machine.TurnCrank();
	EXPECT_EQ(GetOutput(),
		"You turned...\nA gumball comes rolling out the slot...\n");

	ReleaseOutput();
}

TEST_F(GumballMachineTest, NormalOperation)
{
	GumballMachine machine(2);
	CaptureOutput();

	machine.InsertQuarter();
	EXPECT_EQ(GetOutput(), "You inserted a quarter\n");
	ClearOutput();

	machine.TurnCrank();
	std::string output = GetOutput();
	EXPECT_TRUE(output.find("You turned...") != std::string::npos);
	EXPECT_TRUE(output.find("A gumball comes rolling out the slot...")
		!= std::string::npos);

	ReleaseOutput();
}

TEST_F(GumballMachineTest, EjectQuarterAndTryTurnCrank)
{
	GumballMachine machine(2);
	CaptureOutput();

	machine.InsertQuarter();
	ClearOutput();

	machine.EjectQuarter();
	EXPECT_EQ(GetOutput(), "Quarter returned\n");
	ClearOutput();

	machine.TurnCrank();
	EXPECT_EQ(GetOutput(),
		"You turned but there's no quarter\nYou need to pay first\n");

	ReleaseOutput();
}

TEST_F(GumballMachineTest, TryTurnCrankWhenOutOfGumballs)
{
	GumballMachine machine(1);
	CaptureOutput();

	machine.InsertQuarter();
	machine.TurnCrank();
	ClearOutput();

	machine.InsertQuarter();
	EXPECT_EQ(
		GetOutput(), "You can't insert a quarter, the machine is sold out\n");
	ClearOutput();

	machine.TurnCrank();
	EXPECT_EQ(GetOutput(),
		"You turned but there's no gumballs\nNo gumball dispensed\n");

	ReleaseOutput();
}

TEST_F(GumballMachineTest, MultipleGumballsDispensing)
{
	GumballMachine machine(3);
	CaptureOutput();

	for (int i = 0; i < 3; ++i)
	{
		machine.InsertQuarter();
		machine.TurnCrank();
		ClearOutput();
	}

	machine.InsertQuarter();
	EXPECT_EQ(
		GetOutput(), "You can't insert a quarter, the machine is sold out\n");

	ReleaseOutput();

	std::string state = machine.ToString();
	EXPECT_TRUE(state.find("Inventory: 0 gumballs") != std::string::npos);
}

TEST_F(GumballMachineTest, StateToStringRepresentations)
{
	GumballMachine machine(1);

	std::string initialState = machine.ToString();
	EXPECT_TRUE(initialState.find("waiting for quarter") != std::string::npos);

	machine.InsertQuarter();
	std::string hasQuarterState = machine.ToString();
	EXPECT_TRUE(
		hasQuarterState.find("waiting for turn of crank") != std::string::npos);

	machine.TurnCrank();
	std::string afterDispenseState = machine.ToString();
	EXPECT_TRUE(afterDispenseState.find("sold out") != std::string::npos);
}

TEST_F(GumballMachineTest, SingleGumballPluralization)
{
	GumballMachine machine(1);
	std::string state = machine.ToString();
	EXPECT_TRUE(state.find("1 gumball") != std::string::npos);
}

TEST_F(GumballMachineTest, MultipleGumballsPluralization)
{
	GumballMachine machine(2);
	std::string state = machine.ToString();
	EXPECT_TRUE(state.find("2 gumballs") != std::string::npos);
}
