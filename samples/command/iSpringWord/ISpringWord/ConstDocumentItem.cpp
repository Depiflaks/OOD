#include "stdafx.h"
#include "ConstDocumentItem.h"


std::shared_ptr<const IImage> CConstDocumentItem::GetImage()const
{
	return nullptr;
}

std::shared_ptr<const IParagraph> CConstDocumentItem::GetParagraph()const
{
	return nullptr;
}
