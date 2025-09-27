#ifndef SUBJECT_H
#define SUBJECT_H

#include "Enum/SubscribeType.h"
#include <map>
#include <memory>
#include <set>
#include <vector>

template <typename T>
class IObserver;

template <typename T>
class IObservable
{
public:
	virtual ~IObservable() = default;
	virtual void Subscribe(SubscribeType type, const std::weak_ptr<IObserver<T>>& observer, int priority = 0) = 0;
	virtual void Unsubscribe(SubscribeType type, const std::weak_ptr<IObserver<T>>& observer) = 0;
	virtual T GetData() const = 0;
};

template <typename T>
class ISubject : public IObservable<T>
{
public:
	virtual void Notify(SubscribeType type) = 0;
};

template <class T>
class Subject : public ISubject<T>
{
public:
	void Subscribe(SubscribeType type, const std::weak_ptr<IObserver<T>>& observer, int priority = 0) override
	{
		auto& observers = m_observersByType[type];

		if (observers.observerSet.find(observer) != observers.observerSet.end())
		{
			return;
		}

		int finalPriority = GetFinalPriority(observers, priority);

		TryInsertObserver(observers, observer, finalPriority);
	}

	void Notify(SubscribeType type) override
	{
		auto it = m_observersByType.find(type);
		if (it == m_observersByType.end())
		{
			return;
		}

		auto& observers = it->second;
		for (auto priorityIt = observers.priorityMap.begin(); priorityIt != observers.priorityMap.end();)
		{
			auto current = priorityIt++;
			UpdateObserver(current->second);
		}
	}

	void Unsubscribe(SubscribeType type, const std::weak_ptr<IObserver<T>>& observer) override
	{
		auto typeIt = m_observersByType.find(type);
		if (typeIt == m_observersByType.end())
		{
			return;
		}

		auto& observers = typeIt->second;
		RemoveObserverFromCollection(observers, observer);

		if (observers.observerSet.empty())
		{
			m_observersByType.erase(typeIt);
		}
	}

private:
	struct ObserverCollection
	{
		std::set<std::weak_ptr<IObserver<T>>, std::owner_less<std::weak_ptr<IObserver<T>>>> observerSet;
		std::map<int, std::weak_ptr<IObserver<T>>> priorityMap;
		std::map<std::weak_ptr<IObserver<T>>, int, std::owner_less<std::weak_ptr<IObserver<T>>>> observerPriority;
	};

	std::map<SubscribeType, ObserverCollection> m_observersByType;

	int GetFinalPriority(const ObserverCollection& observers, int priority) const
	{
		int finalPriority = priority;
		while (observers.priorityMap.find(finalPriority) != observers.priorityMap.end())
		{
			++finalPriority;
		}
		return finalPriority;
	}

	void TryInsertObserver(ObserverCollection& observers, const std::weak_ptr<IObserver<T>>& observer, int priority)
	{
		auto insertedObserver = observers.observerSet.insert(observer);
		try
		{
			observers.priorityMap.insert({ priority, observer });
			observers.observerPriority.insert({ observer, priority });
		}
		catch (...)
		{
			observers.observerSet.erase(insertedObserver.first);
			observers.priorityMap.erase(priority);
			observers.observerPriority.erase(observer);
			throw;
		}
	}

	void RemoveObserverFromCollection(ObserverCollection& observers, const std::weak_ptr<IObserver<T>>& observer)
	{
		auto priorityIt = observers.observerPriority.find(observer);
		if (priorityIt != observers.observerPriority.end())
		{
			observers.priorityMap.erase(priorityIt->second);
			observers.observerPriority.erase(priorityIt);
		}
		observers.observerSet.erase(observer);
	}

	void UpdateObserver(const std::weak_ptr<IObserver<T>>& observer)
	{
		if (auto shared_observer = observer.lock())
		{
			shared_observer->Update(*this);
		}
		else
		{
			UnsubscribeObserver(observer);
		}
	}

	void UnsubscribeObserver(const std::weak_ptr<IObserver<T>>& observer)
	{
		std::vector<SubscribeType> typesToRemove;

		for (auto& [type, observers] : m_observersByType)
		{
			RemoveObserverFromCollection(observers, observer);

			if (observers.observerSet.empty())
			{
				typesToRemove.push_back(type);
			}
		}
		for (auto type : typesToRemove)
		{
			m_observersByType.erase(type);
		}
	}
};

#endif /* SUBJECT_H */
