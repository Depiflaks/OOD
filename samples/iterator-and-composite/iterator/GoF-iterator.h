#pragma once

#include <memory>

template <typename T>
class IIterator
{
public:
	typedef T ValueType;

	virtual bool HasNext()const = 0;

	virtual ValueType& Get()const = 0;

	virtual void Next() = 0;

	virtual std::unique_ptr<IIterator<ValueType>> Clone()const = 0;

	virtual ~IIterator() = default;
};

template <typename T>
class CIteratorWrapper
{
public:
	typedef IIterator<T> IteratorType;
	CIteratorWrapper(std::unique_ptr<IteratorType> && it)
		: m_it(move(it))
	{
	}

	CIteratorWrapper(const CIteratorWrapper & w)
		: m_it(w.m_it->Clone())
	{
	}

	CIteratorWrapper& operator=(const CIteratorWrapper& w)
	{
		if (this != &w)
		{
			m_it = w.m_it->Clone();
		}
		return *this;
	}

	bool HasNext()const
	{
		return m_it->HasNext();
	}

	T& Get()const
	{
		return m_it->Get();
	}

	void Next()
	{
		return m_it->Next();
	}
private:
	std::unique_ptr<IteratorType> m_it;
};