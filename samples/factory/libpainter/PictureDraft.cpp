#include "stdafx.h"
#include "PictureDraft.h"
using namespace std;

bool CPictureDraft::IsEmpty() const
{
	return m_shapes.empty();
}

CPictureDraft::ConstIterator CPictureDraft::begin() const
{
	return m_shapes.begin();
}

CPictureDraft::ConstIterator CPictureDraft::end() const
{
	return m_shapes.end();
}

void CPictureDraft::AddShape(std::unique_ptr<CShape>&& shape)
{
	m_shapes.push_back(move(shape));
}

CPictureDraft::CPictureDraft()
{
}

