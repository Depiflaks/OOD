#pragma once

#include "./Concepts.h"
#include "./Subject.h"
#include <iostream>
#include <limits>
#include <string_view>
#include <sys/stat.h>

template <typename T>
class IObserver
{
public:
	virtual ~IObserver() = default;
	virtual void Update(const IObservable<T>& observable) = 0;
};

template <typename T>
class AbstractObserver : public IObserver<T>
{
public:
	AbstractObserver(const IObservable<T>& observable)
		: m_observable(static_cast<const void*>(&observable))
	{
	}

protected:
	void ChangeObservable(const IObservable<T>& observable)
	{
		m_observable = static_cast<const void*>(&observable);
	}

	void* GetObservable() const
	{
		return const_cast<void*>(m_observable);
	}

	bool CheckSameObservable(const IObservable<T>& observable) const
	{
		return m_observable == static_cast<const void*>(&observable);
	}

private:
	const void* m_observable;
};

template <typename T>
class MonoAbstractObserver : public AbstractObserver<T>
{
public:
	MonoAbstractObserver(const IObservable<T>& observable)
		: AbstractObserver<T>(observable)
	{
	}

	void ChangeObservable(const IObservable<T>& observable)
	{
		AbstractObserver<T>::ChangeObservable(observable);
	}

	void Update(const IObservable<T>& observable) override
	{
		if (this->CheckSameObservable(observable))
		{
			UpdateObserver(observable);
		}
	}

protected:
	virtual void UpdateObserver(const IObservable<T>& observable) = 0;
};

template <typename T1, typename T2>
class DuoAbstractObserver : public AbstractObserver<T1>
	, public AbstractObserver<T2>
{
public:
	DuoAbstractObserver(const IObservable<T1>& first, const IObservable<T2>& second)
		: AbstractObserver<T1>(first)
		, AbstractObserver<T2>(second)
	{
	}

	void ChangeObservables(const IObservable<T1>& first, const IObservable<T2>& second)
	{
		AbstractObserver<T1>::ChangeObservable(first);
		AbstractObserver<T2>::ChangeObservable(second);
	}

	void Update(const IObservable<T1>& observable) override
	{
		if (AbstractObserver<T1>::CheckSameObservable(observable))
		{
			UpdateFirst(observable);
		}
	}

	void Update(const IObservable<T2>& observable) override
	{
		if (AbstractObserver<T2>::CheckSameObservable(observable))
		{
			UpdateSecond(observable);
		}
	}

protected:
	virtual void UpdateFirst(const IObservable<T1>& observable) = 0;
	virtual void UpdateSecond(const IObservable<T2>& observable) = 0;
};


