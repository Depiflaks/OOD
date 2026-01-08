package view

import "vector-editor/src/geometry"

type IdleState struct {
	c CanvasView
}

func (s *IdleState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	shape.Select(ctrl)
	s.c.setState(s.c.dragging)
}

func (s *IdleState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.ClearSelection()
}

func (s *IdleState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *IdleState) OnMouseMove(e mouseEvent) {}

func (s *IdleState) OnMouseUp(e mouseEvent) {}

// Dragging

type DraggingState struct {
	c CanvasView

	active     *ShapeView
	startMouse geometry.Point
}

func (s *DraggingState) Begin(e mouseEvent, active *ShapeView) {
	s.active = active
	s.startMouse = e.Pos
}

func (s *DraggingState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *DraggingState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *DraggingState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *DraggingState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}
	d := geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}
	s.active.Drag(d)
	s.c.Invalidate()
}

func (s *DraggingState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopDragging()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.Invalidate()
}

type ResizingState struct {
	c CanvasView

	active     *ShapeView
	marker     ResizeMarker
	startMouse geometry.Point
	startSel   geometry.Rect
}

func (s *ResizingState) Begin(e mouseEvent, active *ShapeView, marker ResizeMarker) {
	s.active = active
	s.marker = marker
	s.startMouse = e.Pos
	s.startSel = s.c.selectionRect()
}

func (s *ResizingState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *ResizingState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *ResizingState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *ResizingState) OnMouseMove(e mouseEvent) {
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
	s.c.Invalidate()
}

func (s *ResizingState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopResizing()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.Invalidate()
}
