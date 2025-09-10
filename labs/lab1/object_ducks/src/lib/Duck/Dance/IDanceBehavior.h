#ifndef LAB01_SIMUDUCK_IDANCEBEHAVIOR_H
#define LAB01_SIMUDUCK_IDANCEBEHAVIOR_H

class IDanceBehavior
{
public:
	virtual ~IDanceBehavior() {};
	virtual void Dance() = 0;
};

#endif
