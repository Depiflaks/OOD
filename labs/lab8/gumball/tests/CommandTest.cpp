//
// Created by smmm on 03.12.2025.
//
#include "CoutMock.h"
#include "lib/Command.h"
#include "lib/GumBallMachine.h"

#include "gtest/gtest.h"

TEST_F(GumballMachineTest, HelpCommand_InvalidUsage_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("help extra"), command::CommandParseError);
}

TEST_F(GumballMachineTest, InsertCommand_InvalidUsage_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("insert extra"), command::CommandParseError);
}

TEST_F(GumballMachineTest, EjectCommand_InvalidUsage_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("eject extra"), command::CommandParseError);
}

TEST_F(GumballMachineTest, TurnCommand_InvalidUsage_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("turn extra"), command::CommandParseError);
}

TEST_F(GumballMachineTest, RefillCommand_NoArgument_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("refill"), command::CommandParseError);
}

TEST_F(GumballMachineTest, RefillCommand_NonNumericArgument_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("refill abc"), command::CommandParseError);
}

TEST_F(GumballMachineTest, RefillCommand_TooManyArguments_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("refill 10 extra"), command::CommandParseError);
}

TEST_F(GumballMachineTest, UnknownCommand_Throws)
{
	GumballMachine machine(3);
	command::CommandFactory factory(machine);

	EXPECT_THROW(factory.Parse("foobar"), command::CommandParseError);
}

TEST_F(GumballMachineTest, HelpCommand_ParsesAndExecutes_PrintsHelpAndState)
{
	GumballMachine machine(2);
	command::CommandFactory factory(machine);

	CaptureOutput();
	auto cmd = factory.Parse("help");
	cmd->Execute(machine);
	ReleaseOutput();

	const auto out = GetOutput();
	const auto expected
		= FormatExpectedCommands(command::GetSupportedCommands())
		+ machine.ToString();

	EXPECT_EQ(out, expected);
}

TEST_F(
	GumballMachineTest, InsertCommand_ParsesAndExecutes_UpdatesStateAndOutput)
{
	GumballMachine machine(2);
	command::CommandFactory factory(machine);

	CaptureOutput();
	auto cmd = factory.Parse("insert");
	cmd->Execute(machine);
	ReleaseOutput();

	const auto out = GetOutput();
	EXPECT_EQ(out, "You inserted a quarter\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 2 gumballs
Coins: 1/5 quarter(s)
Machine is waiting for turn of crank
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, EjectCommand_ParsesAndExecutes_UpdatesStateAndOutput)
{
	GumballMachine machine(2);
	command::CommandFactory factory(machine);

	machine.InsertQuarter();
	ClearOutput();

	CaptureOutput();
	auto cmd = factory.Parse("eject");
	cmd->Execute(machine);
	ReleaseOutput();

	const auto out = GetOutput();
	EXPECT_EQ(out,
		"1 quarter(s) returned\n"
		"All quarters returned\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 2 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}

TEST_F(GumballMachineTest, TurnCommand_ParsesAndExecutes_UpdatesStateAndOutput)
{
	GumballMachine machine(2);
	command::CommandFactory factory(machine);

	machine.InsertQuarter();
	ClearOutput();

	CaptureOutput();
	auto cmd = factory.Parse("turn");
	cmd->Execute(machine);
	ReleaseOutput();

	const auto out = GetOutput();
	EXPECT_EQ(out,
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

TEST_F(
	GumballMachineTest, RefillCommand_ParsesAndExecutes_UpdatesStateAndOutput)
{
	GumballMachine machine(0);
	command::CommandFactory factory(machine);

	CaptureOutput();
	auto cmd = factory.Parse("refill 3");
	cmd->Execute(machine);
	ReleaseOutput();

	const auto out = GetOutput();
	EXPECT_EQ(out, "Append 3 gumballs\n");

	const std::string expectedState = R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: 3 gumballs
Coins: 0/5 quarter(s)
Machine is waiting for quarter
)";
	EXPECT_EQ(machine.ToString(), expectedState);
}
