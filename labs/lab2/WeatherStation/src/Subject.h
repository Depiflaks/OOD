#ifndef SUBJECT_H
#define SUBJECT_H

#include <memory>
#include <set>
template <typename T>
class IObserver;

template <typename T>
class ISubject
{
public:
	virtual ~ISubject() = default;
	virtual void RegisterObserver(std::weak_ptr<IObserver<T>> observer) = 0;
	virtual void NotifyObservers() = 0;
	virtual void RemoveObserver(std::weak_ptr<IObserver<T>> observer) = 0;
	virtual T GetData() const = 0;
};

template <class T>
class Subject : public ISubject<T>
{
public:
	typedef IObserver<T> ObserverType;

	void RegisterObserver(std::weak_ptr<ObserverType> observer) override
	{
		m_observers.insert(observer);
	}

	void NotifyObservers() override
	{
		T data = GetData();
		for (auto& observer : m_observers)
		{
			if (std::shared_ptr<ObserverType> observerPtr = observer.lock())
			{
				observerPtr->Update();
			}
			else
			{
				RemoveObserver(observer);
			}
		}
	}

	void RemoveObserver(std::weak_ptr<ObserverType> observer) override
	{
		m_observers.erase(observer);
	}

	virtual T GetData() const = 0;

private:
	std::set<std::weak_ptr<ObserverType>, std::owner_less<std::weak_ptr<ObserverType>>> m_observers;

	static void CheckObserverExist(std::weak_ptr<ObserverType> observer)
	{
		if (observer.expired())
		{
			throw std::runtime_error{ "Observer no longer exists" };
		}
	}
};
#endif /* SUBJECT_H */

