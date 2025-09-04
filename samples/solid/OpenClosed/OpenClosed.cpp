// OpenClosed.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

class Logger
{
public:
	void Log(string message)
	{
		// TODO: Сохранить лог в файле
		(void)&message;
	}
};

class DBLogger
{
public:
	void Log(string message)
	{
		// TODO: Сохранить лог в БД
		(void)&message;
	}
};

class SMTPMailer
{
public:
	void SendMail(string recepient, string text)
	{
		m_logger.Log("Sending '" + text + "' to " + recepient);
		/* Отправляем сообщение */
	}

private:
	Logger m_logger;
};

class ILogger
{
public:
	virtual ~ILogger() = default;
	virtual void Log(string message) = 0;
};

class SMTPMailer2
{
public:
	SMTPMailer2(shared_ptr<ILogger> logger)
		: m_logger(move(logger))
	{
	}
	void SendMail(string recepient, string text)
	{
		m_logger->Log("Sending '" + text + "' to " + recepient);
		/* Отправляем сообщение */
	}

private:
	shared_ptr<ILogger> m_logger;
};

class IList
{
public:
	virtual ~IList() = default;
	virtual void AddItem(int) = 0;
	virtual size_t GetCount() const = 0;
};
class List : public IList
{
	vector<int> m_items;
public:
	void AddItem(int x) override { m_items.push_back(x); }
	size_t GetCount() const override { return m_items.size(); }
};

class DoubleList : public IList
{
	List m_list;
public:
	void AddItem(int x) override
	{
		m_list.AddItem(x);
		m_list.AddItem(x);
	}
	size_t GetCount() const override
	{
		return m_list.GetCount();
	}
};

int main()
{
	return 0;
}
