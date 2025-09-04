#include "stdafx.h"
#include "ChartView.h"


CChartView::CChartView()
{
}

CChartView::~CChartView()
{
}

void CChartView::SetData(const Points2D& data)
{
	m_points = data;
	UpdateBounds();
	RedrawWindow();
}

void CChartView::UpdateBounds()
{
	m_leftTop = m_rightBottom = Point2D(0, 0);
	if (!m_points.empty())
	{
		m_leftTop = m_points.front();
		m_rightBottom = m_points.front();
		for (auto & pt : m_points)
		{
			m_leftTop.x = min(m_leftTop.x, pt.x);
			m_rightBottom.x = max(m_rightBottom.x, pt.x);
			m_leftTop.y = min(m_leftTop.y, pt.y);
			m_rightBottom.y = max(m_rightBottom.y, pt.y);
		}
		if (m_leftTop.y == m_rightBottom.y)
		{
			m_leftTop.y -= 1;
			m_rightBottom.y += 1;
		}
		if (m_leftTop.x == m_rightBottom.x)
		{
			m_leftTop.x -= 1;
			m_rightBottom.x += 1;
		}
	}
}

void CChartView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_points.empty())
	{
		return;
	}

	CRect rc = lpDrawItemStruct->rcItem;
	int w = rc.Width();
	int h = rc.Height();
	double graphWidth = m_rightBottom.x - m_leftTop.x;
	double graphHeight = m_rightBottom.y - m_leftTop.y;
	double scaleX = w / graphWidth;
	double scaleY = h / graphHeight;

	auto TransformX = [&](double x) {
		return int((x - m_leftTop.x) * scaleX);
	};
	auto TransformY = [&](double y) {
		return int((y - m_leftTop.y) * scaleY);
	};

	auto dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	dc->FillSolidRect(0, 0, w, h, RGB(255, 255, 255));

	dc->MoveTo(TransformX(0), TransformY(m_leftTop.y));
	dc->LineTo(TransformX(0), TransformY(m_rightBottom.y));

	dc->MoveTo(TransformX(m_leftTop.x), TransformY(0));
	dc->LineTo(TransformX(m_rightBottom.x), TransformY(0));

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

	auto oldPen = dc->SelectObject(&pen);

	dc->MoveTo(TransformX(m_points.front().x), TransformY(m_points.front().y));
	for (size_t i = 1; i < m_points.size(); ++i)
	{
		auto & pt = m_points[i];
		dc->LineTo(TransformX(pt.x), TransformY(pt.y));
	}

	dc->SelectObject(oldPen);
}
