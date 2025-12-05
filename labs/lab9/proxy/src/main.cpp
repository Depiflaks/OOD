//
// Created by smmm on 05.12.2025.
//
#include "lib/Drawer.h"

#include <iostream>
#include "lib/CoW.h"

struct foo
{
	int a;
	void SetA(int na)
	{
		a = na;
	}
};

constexpr int STAR_COUNT = 20;

int main()
{
	auto a1 = CoW<foo>(foo{});
	auto a2 = a1;

	std::cout << a1.GetInstanceCount() << "\n";
	std::cout << a2.GetInstanceCount() << "\n";

	a2--->SetA(2);
	std::cout << a1.GetInstanceCount() << "\n";
	std::cout << a2.GetInstanceCount() << "\n";
}