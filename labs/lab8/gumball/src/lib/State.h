//
// Created by smmm on 03.12.2025.
//

#ifndef OOD_STATE_H
#define OOD_STATE_H

#include <string>

struct IState
{
	virtual void InsertQuarter() = 0;
	virtual void EjectQuarter() = 0;
	virtual void TurnCrank() = 0;
	virtual void Dispense() = 0;
	virtual std::string ToString() const = 0;
	virtual ~IState() = default;
};

#endif // OOD_STATE_H
