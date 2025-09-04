#pragma once

class IMainDlgController
{
public:
	virtual void SetCoeffA(double a) = 0;
	virtual void SetCoeffB(double b) = 0;
	virtual void SetCoeffC(double c) = 0;
	virtual ~IMainDlgController() = default;
};

