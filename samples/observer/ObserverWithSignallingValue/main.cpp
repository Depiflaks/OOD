#include <functional>
#include <map>
#include <iostream>
#include <boost/noncopyable.hpp>
#include "SignallingValue.h"

using namespace std;
namespace ph = std::placeholders;

class ICurrencyRate
{
public:
	using RateSignal = Signal<void(double rate)>;

	virtual Connection DoOnRateChange(const RateSignal::slot_type & slot) = 0;
	virtual double GetRate()const = 0;

	virtual ~ICurrencyRate() = default;
};

class Stock : public ICurrencyRate
{
public:
	void SetRate(double rate)
	{
		m_rubToUSD = rate;
	}

	double GetRate() const override
	{
		return m_rubToUSD;
	}

	Connection DoOnRateChange(const RateSignal::slot_type & slot) override
	{
		return m_rubToUSD.Connect1(slot, false);
	}

private:
	SignallingValue<double> m_rubToUSD { 59.0 };
};

class AverageCurrencyRateMonitor : boost::noncopyable
{
public:
	AverageCurrencyRateMonitor(ICurrencyRate & cr)
	{
		m_rateChangeConnection = cr.DoOnRateChange(bind(&AverageCurrencyRateMonitor::OnCurrencyRateChange, this, ph::_1));
	}

	double GetAverageRate()const
	{
		return (m_count != 0) ? m_accRate / m_count : 0;
	}
private:
	void OnCurrencyRateChange(double newRate)
	{
		m_accRate += newRate;
		++m_count;
		cout << "Average rate is: " << GetAverageRate() << endl;
	}

	double m_accRate = 0.0;
	int m_count = 0;
	ScopedConnection m_rateChangeConnection;
};


class TV
{
public:
	using ChannelSignal = Signal<void(int)>;

	void SetChannel(int ch)
	{
		m_channel = ch;
	}

	int GetChannel()const
	{
		return m_channel;
	}

	Connection DoOnChannelChange(const ChannelSignal::slot_type& slot)
	{
		return m_channel.Connect1(slot, false);
	}
private:
	SignallingValue<int> m_channel;
};


int main()
{
	Stock s;

	s.DoOnRateChange([](double rate) {
		cout << "Rate is: " << rate << " RUR/USD" << endl;
	});

	s.SetRate(70);
	s.SetRate(59);
	s.SetRate(80);

	{
		AverageCurrencyRateMonitor avgRateMonitor(s);
		s.SetRate(60);
		s.SetRate(70);
		cout << "------------" << endl;
	}
	string ss;
	s.SetRate(80);
	s.SetRate(90);
	s.SetRate(50);

	TV tv;
	tv.DoOnChannelChange([&tv](int ch) {
		if (ch != 1)
		{
			cout << "I want to watch channel #1\n";
			tv.SetChannel(1);
		}
	});
	tv.SetChannel(1);

	tv.SetChannel(8);

	cout << "Current channel is: " << tv.GetChannel() << "\n";

	return 0;
}