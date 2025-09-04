#pragma once

#include <string>

class IVeggies
{
public:
	virtual std::string ToString()const = 0;

	virtual ~IVeggies() = default;
};

class CBlackOlives : public IVeggies 
{
public:
	std::string ToString() const override
	{
		return "Black Olives";
	}
};

class CGarlic : public IVeggies
{
public:
	virtual std::string ToString() const override
	{
		return "Garlic";
	}
};

class COnion : public IVeggies
{
public:
	std::string ToString() const override
	{
		return "Onion";
	}
};

class CMushroom : public IVeggies
{
public:
	std::string ToString() const override
	{
		return "Mushrooms";
	}

};

class CRedPepper : public IVeggies
{
public:
	std::string ToString() const override
	{
		return "Red Pepper";
	}
};

class CSpinach : public IVeggies
{
public:
	std::string ToString() const override
	{
		return "Spinach";
	}
};

class CEggplant : public IVeggies
{
public:
	std::string ToString() const override
	{
		return "Eggplant";
	}
};
