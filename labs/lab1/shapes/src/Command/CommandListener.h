#ifndef COMMANDLISTENER_H
#define COMMANDLISTENER_H

#include <iostream>
#include <string>

class ICommandListener
{
public:
	virtual ~ICommandListener() = default;
	virtual std::string GetLine() = 0;
};

class StdCommandListener : public ICommandListener
{
public:
	std::string GetLine() override
	{
		std::string line;
		std::getline(std::cin, line);
		return line;
	}
};

#endif /* COMMANDLISTENER_H */
