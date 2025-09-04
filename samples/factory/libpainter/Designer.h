#pragma once
#include <iosfwd>

class CPictureDraft;

struct IShapeFactory;

class CDesigner
{
public:
	CDesigner(IShapeFactory & factory);
	~CDesigner();
	CPictureDraft CreateDraft(std::istream & inputData);
private:
	IShapeFactory & m_factory;
};

