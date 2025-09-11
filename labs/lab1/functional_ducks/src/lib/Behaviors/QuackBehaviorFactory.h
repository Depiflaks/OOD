#ifndef QUACK_BEHAVIOR_FACTORY_H
#define QUACK_BEHAVIOR_FACTORY_H

#include <cassert>
#include <functional>
#include <iostream>

using QuackBehavior = std::function<void()>;

class QuackBehaviorFactory final
{
public:
	static QuackBehavior CreateQuackBehavior()
	{
		return []() mutable -> void {
			std::cout << "Quaaaaack!!";
		};
	}

	static QuackBehavior CreateSqueakBehavior()
	{
		return []() mutable -> void {
			std::cout << "Squeak!!";
		};
	}

	static QuackBehavior CreateMuteQuackBehavior()
	{
		return []() mutable -> void {
		};
	}

private:
	QuackBehaviorFactory()
	{
	}
};

#endif
