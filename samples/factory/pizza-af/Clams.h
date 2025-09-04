#pragma once
#include <string>

class IClams
{
public:
	virtual std::string ToString()const = 0;

	virtual ~IClams() = default;
};

// CFreshClams
class CFreshClams : public IClams
{
public:
	virtual std::string ToString() const override
	{
		return "Fresh Clams from Long Island Sound";
	}
};

class CFrozenClams : public IClams
{
public:
	virtual std::string ToString() const override
	{
		return "Frozen Clams";
	}
};