#ifndef DUCK_H
#define DUCK_H

#include "Behaviors/DanceBehaviorFactory.h"
#include "Behaviors/QuackBehaviorFactory.h"
#include "Behaviors/FlyBehaviorFactory.h"

#include <cassert>
#include <iostream>

class Duck
{
public:
	Duck(FlyBehavior flyBehavior,
		QuackBehavior quackBehavior,
		DanceBehavior danceBehavior)
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
		m_quackBehavior();
	}

	void Swim()
	{
		std::cout << "I'm swimming" << std::endl;
	}

	void Fly()
	{
		int flyCount;
		flyCount = m_flyBehavior();
		QuackWithPleasure(flyCount);
	}

	void Dance()
	{
		m_danceBehavior();
	}

	void SetDanceBehavior(DanceBehavior danceBehavior)
	{
		assert(danceBehavior);
		m_danceBehavior = std::move(danceBehavior);
	}

	void SetFlyBehavior(FlyBehavior flyBehavior)
	{
		assert(flyBehavior);
		m_flyBehavior = std::move(flyBehavior);
	}

	virtual void Display() const = 0;
	virtual ~Duck() = default;

private:
	FlyBehavior m_flyBehavior;
	QuackBehavior m_quackBehavior;
	DanceBehavior m_danceBehavior;

	void QuackWithPleasure(int flyCount)
	{
		if (flyCount != 0 && flyCount % 2 == 0)
		{
			Quack();
		}
	}
};

#endif
