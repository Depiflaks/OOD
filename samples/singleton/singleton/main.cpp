#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <mutex>

using namespace std;

class CMeyersSingleton
{
public:
	static CMeyersSingleton & GetInstance()
	{
		static CMeyersSingleton Instance;
		return Instance;
	}

	void DoSomething()
	{
		cout << "Doing something" << endl;
	}

	// Запрещаем копирование и присваивание экземпляров
	CMeyersSingleton(const CMeyersSingleton &) = delete;
	CMeyersSingleton & operator=(const CMeyersSingleton &) = delete;
private:
	CMeyersSingleton()
	{
	}
};

struct ILogger
{
	virtual void Enable(bool enable) = 0;
	virtual void LogMessage(const string& msg) = 0;

	virtual ~ILogger() = default;
	ILogger(const ILogger&) = delete;
	ILogger& operator=(const ILogger&) = delete;
protected:
	ILogger() = default;
};

ILogger & GetDefaultLogger()
{
	static struct Logger : ILogger
	{
		void LogMessage(const string& msg) override
		{
			if (m_enabled)
			{
				if (!m_strm.is_open())
				{
					m_strm.open("log.txt", ios::app);
				}
				m_strm << msg << endl;
				m_strm.flush();
			}
		}

		void Enable(bool enable) override
		{
			if (m_enabled != enable)
			{
				m_enabled = enable;
				if (!enable && m_strm.is_open())
				{
					m_strm.close();
				}
			}
		}

		ofstream m_strm;
		bool m_enabled = false;
	} logger;

	return logger;
}

template<class T>
class CSingleton
{
public:
	static T& Instance()
	{
		call_once(m_flag, [] {
			m_instance.reset(new T());
		});
		return *m_instance;
	}
protected:
	~CSingleton() {}
	CSingleton() {}
private:
	CSingleton(const CSingleton&) = delete;
	CSingleton& operator=(const CSingleton&) = delete;

	static unique_ptr<T> m_instance;
	static once_flag m_flag;
};
template<class T> unique_ptr<T> CSingleton<T>::m_instance;
template<class T> once_flag CSingleton<T>::m_flag;

class CMyClass : public CSingleton<CMyClass>
{
	friend class CSingleton<CMyClass>;
public:
	void DoSomething()
	{
		cout << "CMyClass::DoSomething" << endl;
	}
private:
	CMyClass()
	{
	}
};

int main()
{
	{
		auto & instance = CMeyersSingleton::GetInstance();
		instance.DoSomething();
	}

	{
		auto & logger = GetDefaultLogger();
		logger.LogMessage("this message will not be logged");
		logger.Enable(true);
		logger.LogMessage("This message will be logged");
	}

	{
		auto & inst = CMyClass::Instance();
		inst.DoSomething();
	}
	return 0;
}