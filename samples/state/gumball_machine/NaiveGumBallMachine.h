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
		SoldOut, // Жвачка закончилась
		NoQuarter, // Нет монетки
		HasQuarters, // Есть монетка
		Sold, // Монетка выдана
	};

	GumballMachine(unsigned count)
		: m_coinCount(count)
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
			m_state = State::HasQuarters;
			break;
		case State::HasQuarters:
			cout << "You can't insert another quarter\n";
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
		case State::HasQuarters:
			cout << "Quarter returned\n";
			m_state = State::NoQuarter;
			break;
		case State::NoQuarter:
			cout << "You haven't inserted a quarter\n";
			break;
		case State::Sold:
			cout << "Sorry you already turned the crank\n";
			break;
		case State::SoldOut:
			cout << "You can't eject, you haven't inserted a quarter yet\n";
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
			cout << "You turned but there's no quarter\n";
			break;
		case State::HasQuarters:
			cout << "You turned...\n";
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
		m_coinCount = numBalls;
		m_state = numBalls > 0 ? State::NoQuarter : State::SoldOut;
	}

	std::string ToString() const
	{
		std::string state = (m_state == State::SoldOut)
			? "sold out"
			: (m_state == State::NoQuarter)	 ? "waiting for quarter"
			: (m_state == State::HasQuarters) ? "waiting for turn of crank"
											 : "delivering a gumball";

		return std::format(R"(
Mighty Gumball, Inc.
C++-enabled Standing Gumball Model #2016
Inventory: {} gumball{}
Machine is {}
)",
			m_coinCount, m_coinCount != 1 ? "s" : "", state);
	}

private:
	void Dispense()
	{
		using namespace std;
		switch (m_state)
		{
		case State::Sold:
			cout << "A gumball comes rolling out the slot\n";
			--m_coinCount;
			if (m_coinCount == 0)
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
			cout << "No gumball dispensed\n";
			break;
		}
	}

	unsigned m_coinCount; // Количество шариков
	State m_state = State::SoldOut;
};
} // namespace naive