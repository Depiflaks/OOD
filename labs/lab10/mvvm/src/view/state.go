package view

import "vector-editor/src/geometry"

type IdleViewState struct {
	c *CanvasView
}

func (s *IdleViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	shape.Select(ctrl)
	s.c.setState(s.c.selected)
	s.c.invalidate()
}

func (s *IdleViewState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.clearSelection()
	s.c.setState(s.c.idle)
}

func (s *IdleViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *IdleViewState) OnMouseMove(e mouseEvent) {}

func (s *IdleViewState) OnMouseUp(e mouseEvent) {}

type SelectedViewState struct {
	c *CanvasView
}

func (s *SelectedViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	if !shape.IsSelected() || ctrl {
		shape.Select(ctrl)
	}
	shape.StartDragging()
	s.c.dragging.Begin(e, shape)
	s.c.setState(s.c.dragging)
	s.c.invalidate()
}

func (s *SelectedViewState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.clearSelection()
	s.c.setState(s.c.idle)
}

func (s *SelectedViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {
	shape.StartResizing()
	s.c.resizing.Begin(e, shape, marker)
	s.c.setState(s.c.resizing)
	s.c.invalidate()
}

func (s *SelectedViewState) OnMouseMove(e mouseEvent) {}

func (s *SelectedViewState) OnMouseUp(e mouseEvent) {}

type DraggingViewState struct {
	c *CanvasView

	active     *ShapeView
	startMouse geometry.Point
}

func (s *DraggingViewState) Begin(e mouseEvent, active *ShapeView) {
	s.active = active
	s.startMouse = e.Pos
}

func (s *DraggingViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *DraggingViewState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *DraggingViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *DraggingViewState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}
	d := geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}
	s.active.Drag(d)
	s.c.invalidate()
}

func (s *DraggingViewState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopDragging()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.invalidate()
}

type ResizingViewState struct {
	c *CanvasView

	active     *ShapeView
	marker     ResizeMarker
	startMouse geometry.Point
	startSel   geometry.Rect
}

func (s *ResizingViewState) Begin(e mouseEvent, active *ShapeView, marker ResizeMarker) {
	s.active = active
	s.marker = marker
	s.startMouse = e.Pos
	s.startSel = s.c.selectionRect()
}

func (s *ResizingViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *ResizingViewState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *ResizingViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *ResizingViewState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}

	dm := geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}

	left := s.startSel.Position.X
	top := s.startSel.Position.Y
	right := s.startSel.Position.X + s.startSel.Size.Width
	bottom := s.startSel.Position.Y + s.startSel.Size.Height

	newLeft := left
	newTop := top
	newRight := right
	newBottom := bottom

	switch s.marker {
	case MarkerTopLeft:
		newLeft = left + dm.X
		newTop = top + dm.Y
	case MarkerTopRight:
		newRight = right + dm.X
		newTop = top + dm.Y
	case MarkerBottomLeft:
		newLeft = left + dm.X
		newBottom = bottom + dm.Y
	case MarkerBottomRight:
		newRight = right + dm.X
		newBottom = bottom + dm.Y
	}

	if newRight-newLeft < 1 {
		newRight = newLeft + 1
	}
	if newBottom-newTop < 1 {
		newBottom = newTop + 1
	}

	newSel := geometry.Rect{
		Position: geometry.Point{X: newLeft, Y: newTop},
		Size:     geometry.Bounds{Width: newRight - newLeft, Height: newBottom - newTop},
	}

	scaleX := newSel.Size.Width / s.startSel.Size.Width
	scaleY := newSel.Size.Height / s.startSel.Size.Height
	scale := geometry.Scale{ScaleX: scaleX, ScaleY: scaleY}

	deltaTL := geometry.Vector{
		X: newSel.Position.X - s.startSel.Position.X,
		Y: newSel.Position.Y - s.startSel.Position.Y,
	}

	s.active.Resize(deltaTL, scale)
	s.c.invalidate()
}

func (s *ResizingViewState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopResizing()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.invalidate()
}
