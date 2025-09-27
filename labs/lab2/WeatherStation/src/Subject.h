#ifndef SUBJECT_H
#define SUBJECT_H

#include <map>
#include <memory>
#include <set>

template <typename T>
class IObserver;

template <typename T>
class IObservable
{
public:
	virtual ~IObservable() = default;
	virtual void RegisterObserver(const std::weak_ptr<IObserver<T>>& observer, int priority = 0) = 0;
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
	void RegisterObserver(const std::weak_ptr<IObserver<T>>& observer, int priority = 0) override
	{
		if (m_observers.find(observer) != m_observers.end())
		{
			return;
		}

		int finalPriority = GetFinalPriority(priority);

		TryInsertObserver(observer, finalPriority);
	}

	void NotifyObservers() override
	{
		auto it = m_priorityMap.begin();
		while (it != m_priorityMap.end())
		{
			auto current = it;
			++it;
			UpdateObserver(current->second);
		}
	}

	void RemoveObserver(const std::weak_ptr<IObserver<T>>& observer) override
	{
		auto priorityIt = m_observerPriority.find(observer);
		if (priorityIt != m_observerPriority.end())
		{
			m_priorityMap.erase(priorityIt->second);
			m_observerPriority.erase(priorityIt);
		}
		m_observers.erase(observer);
	}

private:
	std::set<std::weak_ptr<IObserver<T>>, std::owner_less<std::weak_ptr<IObserver<T>>>> m_observers;
	std::map<int, std::weak_ptr<IObserver<T>>> m_priorityMap;
	std::map<std::weak_ptr<IObserver<T>>, int, std::owner_less<std::weak_ptr<IObserver<T>>>> m_observerPriority;

	void TryInsertObserver(const std::weak_ptr<IObserver<T>>& observer, int priority)
	{
		auto insertedObserver = m_observers.insert(observer);
		try
		{
			m_priorityMap.insert({ priority, observer });
			m_observerPriority.insert({ observer, priority });
		}
		catch (...)
		{
			m_observers.erase(insertedObserver.first);
			m_priorityMap.erase(priority);
			m_observerPriority.erase(observer);
			throw;
		}
	}

	int GetFinalPriority(int priority) const
	{
		int finalPriority = priority;
		while (m_priorityMap.find(finalPriority) != m_priorityMap.end())
		{
			++finalPriority;
		}
		return finalPriority;
	}

	void UpdateObserver(const std::weak_ptr<IObserver<T>>& observer)
	{
		if (auto shared_observer = observer.lock())
		{
			shared_observer->Update(*this);
		}
		else
		{
			RemoveObserver(observer);
		}
	}
};

#endif /* SUBJECT_H */
