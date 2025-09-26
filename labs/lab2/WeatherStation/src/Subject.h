#ifndef SUBJECT_H
#define SUBJECT_H

#include <memory>
#include <set>
template <typename T>
class IObserver;

template <typename T>
class IObservable
{
public:
	virtual ~IObservable() = default;
	virtual void RegisterObserver(const std::weak_ptr<IObserver<T>>& observer) = 0;
	virtual void RemoveObserver(const std::weak_ptr<IObserver<T>>& observer) = 0;
	virtual T GetData() const = 0;
};

template <typename T>
class ISubject : public IObservable<T>
{
public:
	virtual void NotifyObservers() = 0;
};

template <class T>
class Subject : public ISubject<T>
{
public:
	typedef IObserver<T> ObserverType;

	void RegisterObserver(const std::weak_ptr<IObserver<T>>& observer) override
	{
		m_observers.insert(observer);
	}

	void NotifyObservers() override
	{
		for (auto observer : m_observers)
		{
			UpdateObserver();
		}
	}

	void RemoveObserver(const std::weak_ptr<IObserver<T>>& observer) override
	{
		m_observers.erase(observer);
	}

private:
	std::set<const ObserverType*> m_observers;

	void UpdateObserver(const std::weak_ptr<IObserver<T>>& observer)
	{
		if (auto shared_observer = observer.lock())
		{
			shared_observer->Update(*this);
		}
		else
		{
			m_observers.erase(observer);
		}
	}
};

#endif /* SUBJECT_H */
