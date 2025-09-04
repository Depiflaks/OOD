#pragma once

#include <string>

class IDough
{
public:
	virtual std::string ToString()const = 0;

	virtual ~IDough() = default;
};

class CThickCrustDough : public IDough
{
public:
	std::string ToString() const override
	{
		return "ThickCrust style extra thick crust dough";
	}
};

class CThinCrustDough : public IDough 
{
public:
	std::string ToString() const override
	{
		return "Thin Crust Dough";
	}
};
