#ifndef DANCE_BEHAVIOR_FACTORY_H
#define DANCE_BEHAVIOR_FACTORY_H

#include <cassert>
#include <functional>
#include <iostream>

using DanceBehavior = std::function<void()>;

class DanceBehaviorFactory final
{
public:
	static DanceBehavior CreateDanceWaltz()
	{
		return []() mutable -> void {
			std::cout << "I'm dancing waltz!\n";
		};
	}
	// TODO: засунуть в пространство имён (мы не в java)

	static DanceBehavior CreateDanceMinuet()
	{
		return []() mutable -> void {
			std::cout << "I'm dancing minuet!\n";
		};
	}

	static DanceBehavior CreateNoDance()
	{
		return []() mutable -> void {
		};
	}

private:
	DanceBehaviorFactory()
	{
	}
};

#endif
