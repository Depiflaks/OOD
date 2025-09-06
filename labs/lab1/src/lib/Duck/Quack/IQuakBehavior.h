#ifndef IQUAKBEHAVIOR_H
#define IQUAKBEHAVIOR_H

class IQuackBehavior
{
public:
	virtual ~IQuackBehavior() {};
	virtual void Quack() = 0;
};

#endif
