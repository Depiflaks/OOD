#pragma once

#include <format>
#include <iostream>

namespace naive
{
class GumballMachine
{
public:
	enum class State
	{
		SoldOut,
		NoQuarter,
		HasQuarters,
		FullQuarters,
		Sold,
	};

	GumballMachine(unsigned count)
		: m_ballCount(count)
		, m_state(count > 0 ? State::NoQuarter : State::SoldOut)
	{
	}

	void InsertQuarter()
	{
		using namespace std;
		switch (m_state)
		{
		case State::SoldOut:
			cout << "You can't insert a quarter, the machine is sold out\n";
			break;
		case State::NoQuarter:
			cout << "You inserted a quarter\n";
			++m_coinCount;
			m_state = State::HasQuarters;
			break;
		case State::HasQuarters:
			if (m_coinCount < k_coinCapacity)
			{
				cout << "Deposit another coin...\n";
				++m_coinCount;
				if (m_coinCount == k_coinCapacity)
				{
					cout << "Coin collector if full now!\n";
					m_state = State::FullQuarters;
				}
			}
			else
			{
				cout << "Coin collector if full now!\n";
				m_state = State::FullQuarters;
			}
			break;
		case State::FullQuarters:
			cout << "You can't insert a quarter, the machine is full of "
					"quarters\n";
			break;
		case State::Sold:
			cout << "Please wait, we're already giving you a gumball\n";
			break;
		}
	}

	void EjectQuarter()
	{
		using namespace std;
		switch (m_state)
		{
		case State::NoQuarter:
			cout << "You haven't inserted any quarters\n";
			break;
		case State::HasQuarters:
		case State::FullQuarters:
		case State::SoldOut:
			cout << m_coinCount << " quarter(s) returned\n";
			m_coinCount = 0;
			cout << "All quarters returned\n";
			m_state = (m_ballCount > 0) ? State::NoQuarter : State::SoldOut;
			break;
		case State::Sold:
			cout << "Sorry you already turned the crank\n";
			break;
		}
	}

	void TurnCrank()
	{
		using namespace std;
		switch (m_state)
		{
		case State::SoldOut:
			cout << "You turned but there's no gumballs\n";
			break;
		case State::NoQuarter:
			cout << "You turned but there's no quarters\n";
			break;
		case State::HasQuarters:
		case State::FullQuarters:
			cout << "Pull the lever, Kronk!\n";
			m_state = State::Sold;
			Dispense();
			break;
		case State::Sold:
			cout << "Turning twice doesn't get you another gumball\n";
			break;
		}
	}

	void Refill(unsigned numBalls)
	{
		using namespace std;
		m_ballCount += numBalls;
		cout << "Append " << numBalls << " gumballs\n";

		if (m_ballCount == 0)
		{
			m_state = State::SoldOut;
			return;
		}

		if (m_coinCount == 0)
		{
			m_state = State::NoQuarter;
		}
		else if (m_coinCount < k_coinCapacity)
		{
			m_state = State::HasQuarters;
		}
		else
		{
			m_state = State::FullQuarters;
		}
	}

	std::string ToString() const
	{
		std::string state = (m_state == State::SoldOut) ? "sold out"
			: (m_state == State::NoQuarter)				? "waiting for quarter"
			: (m_state == State::HasQuarters)  ? "waiting for turn of crank"
			: (m_state == State::FullQuarters) ? "full of quarters"
											   : "delivering a gumball";

		return std::format(R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2025
Inventory: {} gumball{}
Coins: {}/{} quarter(s)
Machine is {}
)",
			m_ballCount, m_ballCount != 1 ? "s" : "", m_coinCount,
			k_coinCapacity, state);
	}

private:
	void Dispense()
	{
		using namespace std;
		switch (m_state)
		{
		case State::Sold:
			if (m_ballCount != 0)
			{
				cout << "A gumball comes rolling out the slot...\n";
				--m_ballCount;
			}
			if (m_ballCount == 0)
			{
				cout << "Oops, out of gumballs\n";
				m_state = State::SoldOut;
			}
			else
			{
				m_state = State::NoQuarter;
			}
			break;
		case State::NoQuarter:
			cout << "You need to pay first\n";
			break;
		case State::SoldOut:
		case State::HasQuarters:
		case State::FullQuarters:
			cout << "No gumball dispensed\n";
			break;
		}
	}

	unsigned m_ballCount = 0;
	unsigned m_coinCount = 0;
	static constexpr unsigned k_coinCapacity = 5;
	State m_state = State::SoldOut;
};
} // namespace naive
