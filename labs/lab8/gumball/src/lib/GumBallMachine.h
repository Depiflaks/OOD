#pragma once
#include <format>
#include <iostream>

#include "State.h"

#include <vector>

struct IGumballMachine
{
	virtual void ReleaseBall() = 0;
	virtual unsigned GetBallCount() const = 0;

	virtual void DepositQuarter() = 0;
	virtual unsigned GetQuartersCount() const = 0;
	virtual void ReturnAllCoins() = 0;
	virtual void AppendBalls(unsigned count) = 0;
	virtual bool CanDepositQuarter() const = 0;

	virtual void SetSoldOutState() = 0;
	virtual void SetNoQuarterState() = 0;
	virtual void SetSoldState() = 0;
	virtual void SetHasQuartersState() = 0;
	virtual void SetFullQuartersState() = 0;

	virtual ~IGumballMachine() = default;
};

class SoldState final : public IState
{
public:
	explicit SoldState(IGumballMachine& gumballMachine)
		: m_gumballMachine(gumballMachine)
	{
	}
	void InsertQuarter() override
	{
		std::cout << "Please wait, we're already giving you a gumball\n";
	}
	void EjectQuarters() override
	{
		std::cout << "Sorry you already turned the crank\n";
	}
	void TurnCrank() override
	{
		std::cout << "Turning twice doesn't get you another gumball\n";
	}
	void Dispense() override
	{
		m_gumballMachine.ReleaseBall();
		if (m_gumballMachine.GetBallCount() == 0)
		{
			std::cout << "Oops, out of gumballs\n";
			m_gumballMachine.SetSoldOutState();
		}
		else if (m_gumballMachine.GetQuartersCount() == 0)
		{
			m_gumballMachine.SetNoQuarterState();
		}
		else
		{
			m_gumballMachine.SetHasQuartersState();
		}
	}
	std::string ToString() const override
	{
		return "delivering a gumball";
	}
	void RefillBalls(unsigned count) override
	{
		std::cout << "Cannot refill machine in this state\n";
	}

private:
	IGumballMachine& m_gumballMachine;
};

class SoldOutState final : public IState
{
public:
	explicit SoldOutState(IGumballMachine& gumballMachine)
		: m_gumballMachine(gumballMachine)
	{
	}

	void InsertQuarter() override
	{
		std::cout << "You can't insert a quarter, the machine is sold out\n";
	}
	void EjectQuarters() override
	{
		m_gumballMachine.ReturnAllCoins();
		std::cout << "All quarters returned\n";
	}
	void TurnCrank() override
	{
		std::cout << "You turned but there's no gumballs\n";
	}
	void Dispense() override
	{
		std::cout << "No gumball dispensed\n";
	}
	std::string ToString() const override
	{
		return "sold out";
	}
	void RefillBalls(unsigned count) override
	{
		m_gumballMachine.AppendBalls(count);
		if (m_gumballMachine.GetQuartersCount() == 0)
		{
			m_gumballMachine.SetNoQuarterState();
		}
		else if (m_gumballMachine.CanDepositQuarter())
		{
			m_gumballMachine.SetHasQuartersState();
		}
		else
		{
			m_gumballMachine.SetFullQuartersState();
		}
	}

private:
	IGumballMachine& m_gumballMachine;
};

class FullQuartersState final : public IState
{
public:
	explicit FullQuartersState(IGumballMachine& gumballMachine)
		: m_gumballMachine(gumballMachine)
	{
	}

	void InsertQuarter() override
	{
		std::cout
			<< "You can't insert a quarter, the machine is full of quarters\n";
	}

	void EjectQuarters() override
	{
		m_gumballMachine.ReturnAllCoins();
		m_gumballMachine.SetNoQuarterState();
		std::cout << "All quarters returned\n";
	}

	void TurnCrank() override
	{
		std::cout << "Pull the lever, Kronk!\n";
		m_gumballMachine.SetSoldState();
	}

	void Dispense() override
	{
		std::cout << "No gumball dispensed\n";
	}

	void RefillBalls(unsigned count) override
	{
		m_gumballMachine.AppendBalls(count);
	}
	std::string ToString() const override
	{
		return "full of quarters";
	}

private:
	IGumballMachine& m_gumballMachine;
};

class HasQuartersState final : public IState
{
public:
	explicit HasQuartersState(IGumballMachine& gumballMachine)
		: m_gumballMachine(gumballMachine)
	{
	}

	void InsertQuarter() override
	{
		if (m_gumballMachine.CanDepositQuarter())
		{
			std::cout << "Deposit another coin...\n";
			m_gumballMachine.DepositQuarter();
		}
		if (!m_gumballMachine.CanDepositQuarter())
		{
			std::cout << "Coin collector if full now!\n";
			m_gumballMachine.SetFullQuartersState();
		}
	}
	void EjectQuarters() override
	{
		m_gumballMachine.ReturnAllCoins();
		m_gumballMachine.SetNoQuarterState();
		std::cout << "All quarters returned\n";
	}
	void TurnCrank() override
	{
		std::cout << "Pull the lever, Kronk!\n";
		m_gumballMachine.SetSoldState();
	}
	void Dispense() override
	{
		std::cout << "No gumball dispensed\n";
	}
	void RefillBalls(unsigned count) override
	{
		m_gumballMachine.AppendBalls(count);
	}
	std::string ToString() const override
	{
		return "waiting for turn of crank";
	}

private:
	IGumballMachine& m_gumballMachine;
};

class NoQuarterState final : public IState
{
public:
	explicit NoQuarterState(IGumballMachine& gumballMachine)
		: m_gumballMachine(gumballMachine)
	{
	}

	void InsertQuarter() override
	{
		m_gumballMachine.DepositQuarter();
		m_gumballMachine.SetHasQuartersState();
		std::cout << "You inserted a quarter\n";
	}
	void EjectQuarters() override
	{
		std::cout << "You haven't inserted any quarters\n";
	}
	void TurnCrank() override
	{
		std::cout << "You turned but there's no quarters\n";
	}
	void Dispense() override
	{
		std::cout << "You need to pay first\n";
	}
	void RefillBalls(unsigned count) override
	{
		m_gumballMachine.AppendBalls(count);
	}
	std::string ToString() const override
	{
		return "waiting for quarter";
		// TODO: добавить сюда вывод возможных действий из текущего состояния
	}

private:
	IGumballMachine& m_gumballMachine;
};

class GumballMachine final : IGumballMachine
{
public:
	explicit GumballMachine(unsigned numBalls)
		: m_ballCount(numBalls)
		, m_soldState(*this)
		, m_soldOutState(*this)
		, m_noQuarterState(*this)
		, m_fullQuartersState(*this)
		, m_hasQuartersState(*this)
		, m_state(&m_soldOutState)
	{
		if (m_ballCount > 0)
		{
			m_state = &m_noQuarterState;
		}
	}

	void RefillMachine(unsigned count)
	{
		m_state->RefillBalls(count);
		std::cout << "Append " << count << " gumballs\n";
	}

	void EjectQuarter()
	{
		m_state->EjectQuarters();
	}

	void InsertQuarter()
	{
		m_state->InsertQuarter();
	}

	void TurnCrank()
	{
		m_state->TurnCrank();
		m_state->Dispense();
	}

	std::string ToString() const
	{
		return std::format(R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: {} gumball{}
Coins: {}/{} quarter(s)
Machine is {}
)",
			m_ballCount, m_ballCount != 1 ? "s" : "", m_coinCount,
			k_coinCapacity, m_state->ToString());
	}

private:
	unsigned GetBallCount() const override
	{
		return m_ballCount;
	}

	void AppendBalls(unsigned count) override
	{
		m_ballCount += count;
		std::cout << "Append " << count << " gumballs\n";
	}

	void ReleaseBall() override
	{
		if (m_ballCount != 0)
		{
			std::cout << "A gumball comes rolling out the slot...\n";
			--m_coinCount;
			--m_ballCount;
		}
	}

	void DepositQuarter() override
	{
		++m_coinCount;
	}

	bool CanDepositQuarter() const override
	{
		return m_coinCount < k_coinCapacity;
	}

	void SetSoldOutState() override
	{
		m_state = &m_soldOutState;
	}

	void SetNoQuarterState() override
	{
		m_state = &m_noQuarterState;
	}

	void SetSoldState() override
	{
		m_state = &m_soldState;
	}

	void SetHasQuartersState() override
	{
		m_state = &m_hasQuartersState;
	}

	void SetFullQuartersState() override
	{
		m_state = &m_fullQuartersState;
	}

	void ReturnAllCoins() override
	{
		std::cout << m_coinCount << " quarter(s) returned\n";
		m_coinCount = 0;
	}

	unsigned GetQuartersCount() const override
	{
		return m_coinCount;
	}

	static constexpr int k_coinCapacity = 5;

	unsigned m_ballCount = 0;
	unsigned m_coinCount = 0;

	SoldState m_soldState;
	SoldOutState m_soldOutState;
	NoQuarterState m_noQuarterState;
	FullQuartersState m_fullQuartersState;
	HasQuartersState m_hasQuartersState;
	IState* m_state = nullptr;
};
