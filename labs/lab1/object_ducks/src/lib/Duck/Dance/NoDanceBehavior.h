#ifndef NODANCEBEHAVIOR_H
#define NODANCEBEHAVIOR_H

#include "IDanceBehavior.h"

class NoDanceBehavior : public IDanceBehavior
{
public:
	void Dance() override {}
};

#endif
