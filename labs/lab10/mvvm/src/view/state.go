package view

import (
	"vector-editor/src/geometry"
)

type idleState struct {
	c CanvasView
}

func NewIdleState(c CanvasView) State {
	return &idleState{c: c}
}

func (s *idleState) OnMouseLeave() {}

func (s *idleState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	if !shape.IsSelected() {
		shape.Select(ctrl)
	}
	s.c.SetDraggingState(e, shape)
}

func (s *idleState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.ClearSelection()
}

func (s *idleState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {
	s.c.SetResizingState(e, shape, marker)
}

func (s *idleState) OnMouseMove(e mouseEvent) {}

func (s *idleState) OnMouseUp(e mouseEvent) {}

// Dragging

func NewDraggingState(c CanvasView, e mouseEvent, active *ShapeView) State {
	active.StartDragging()
	return &draggingState{
		c:          c,
		active:     active,
		startMouse: e.Pos,
	}
}

type draggingState struct {
	c CanvasView

	active     *ShapeView
	startMouse geometry.Point
}

func (s *draggingState) OnMouseLeave() {
	if s.active != nil {
		s.active.StopDragging()
	}
	s.c.SetIdleState()
}

func (s *draggingState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *draggingState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *draggingState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *draggingState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}
	d := s.findDelta(e)
	s.active.Drag(d)
}

func (s *draggingState) findDelta(e mouseEvent) geometry.Vector {
	return geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}
}

func (s *draggingState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopDragging()
		s.OnMouseMove(e)
	}
	s.active = nil
	s.c.SetIdleState()
}

func NewResizingState(c CanvasView, e mouseEvent, active *ShapeView, marker ResizeMarker) State {
	active.StartResizing()
	return &resizingState{
		c:          c,
		active:     active,
		marker:     marker,
		startMouse: e.Pos,
		startSel:   c.SelectionRect(),
	}
}

type resizingState struct {
	c CanvasView

	active     *ShapeView
	marker     ResizeMarker
	startMouse geometry.Point
	startSel   geometry.Rect
}

func (s *resizingState) OnMouseLeave() {
	if s.active != nil {
		s.active.StopResizing()
	}
	s.c.SetIdleState()
}

func (s *resizingState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *resizingState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *resizingState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *resizingState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}

	scale, deltaTL := s.findScaleAndDirection(e)
	s.active.Resize(deltaTL, scale)
}

func (s *resizingState) findScaleAndDirection(e mouseEvent) (geometry.Scale, geometry.Vector) {
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
		newRight = newLeft + 20
	}
	if newBottom-newTop < 1 {
		newBottom = newTop + 20
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
	return scale, deltaTL
}

func (s *resizingState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopResizing()
		s.OnMouseMove(e)
	}
	s.c.SetIdleState()
}
