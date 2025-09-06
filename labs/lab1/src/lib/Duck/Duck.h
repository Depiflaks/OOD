#ifndef DUCK_H
#define DUCK_H

#include "Dance/IDanceBehavior.h"
#include "Fly/IFlyBehavior.h"
#include "Quack/IQuakBehavior.h"

#include <cassert>
#include <iostream>
#include <memory>

class Duck
{
public:
	Duck(std::unique_ptr<IFlyBehavior>&& flyBehavior,
		std::unique_ptr<IQuackBehavior>&& quackBehavior,
		std::unique_ptr<IDanceBehavior>&& danceBehavior)
		: m_quackBehavior(std::move(quackBehavior))
		, m_flyBehavior(std::move(flyBehavior))
		, m_danceBehavior(std::move(danceBehavior))
	{
		assert(m_quackBehavior);
		assert(m_flyBehavior);
		assert(m_danceBehavior);
	}

	void Quack()
	{
		m_quackBehavior->Quack();
	}

	void Swim()
	{
		std::cout << "I'm swimming" << std::endl;
	}

	void Fly()
	{
		m_flyBehavior->Fly();
		QuackWithPleasure();
	}

	void Dance()
	{
		m_danceBehavior->Dance();
	}

	void SetDanceBehavior(std::unique_ptr<IDanceBehavior>&& danceBehavior)
	{
		assert(danceBehavior);
		m_danceBehavior = std::move(danceBehavior);
	}

	void SetFlyBehavior(std::unique_ptr<IFlyBehavior>&& flyBehavior)
	{
		assert(flyBehavior);
		m_flyBehavior = std::move(flyBehavior);
	}

	virtual void Display() const = 0;
	virtual ~Duck() = default;

private:
	std::unique_ptr<IFlyBehavior> m_flyBehavior;
	std::unique_ptr<IQuackBehavior> m_quackBehavior;
	std::unique_ptr<IDanceBehavior> m_danceBehavior;

	void QuackWithPleasure()
	{
		if (m_flyBehavior->CanFly() && m_flyBehavior->GetFlyCount() % 2 == 0)
		{
			Quack();
		}
	}
	// TODO: вынести в отдельную стратегию случай расширения с другим контекстом
};

#endif
