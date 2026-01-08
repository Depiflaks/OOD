package modelview

import (
	"vector-editor/src/geometry"
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

// EditableShape

func (s *shapeModelView) GetShape() model.Shape {
	return s.shape
}

func (s *shapeModelView) Move(delta geometry.Vector) {
	pos := s.dragStartPosition
	pos.X += delta.X
	pos.Y += delta.Y
	s.UpdateRect(pos, s.size)
}

func (s *shapeModelView) Scale(delta geometry.Vector, scale geometry.Scale) {
	startPos := s.resizeStartPosition
	startBounds := s.resizeStartBounds
	newPos, newBounds := geometry.CalculateScale(delta, scale, startPos, startBounds)
	s.UpdateRect(newPos, newBounds)
}

// ViewEvents

func (s *shapeModelView) StartDragging() {
	if !s.isDragging {
		s.isDragging = true
		s.dragStartPosition = s.position
		s.manager.StartDragging()
	}
}

func (s *shapeModelView) StopDragging() {
	if s.isDragging {
		s.isDragging = false
		s.manager.StopDragging()
	}
}

func (s *shapeModelView) StartResizing() {
	if !s.isResizing {
		s.isResizing = true
		s.resizeStartPosition = s.position
		s.resizeStartBounds = s.size
		s.manager.StartResizing()
	}
}

func (s *shapeModelView) StopResizing() {
	if s.isResizing {
		s.isResizing = false
		s.manager.StopResizing()
	}
}

// Observer

func (s *shapeModelView) UpdateRect(position geometry.Point, bounds geometry.Bounds) {
	s.position = position
	s.size = bounds
	s.notify()
}

func (s *shapeModelView) UpdateStyle(style geometry.Style) {
	s.style = style
	s.manager.SetStyle(style)
	s.notify()
}

type ShapeModelView interface {
	manager.ViewEvents

	IsDeleted() bool
	IsSelected() bool

	SetDeleted(deleted bool)
	Select(withCtrl bool)
	Drag(delta geometry.Vector)
	Resize(delta geometry.Vector, scale geometry.Scale)

	GetBounds() geometry.Bounds
	GetPosition() geometry.Point
	GetStyle() geometry.Style
	GetShapeType() model.ShapeType

	AddObserver(o ShapeModelViewObserver)
}

type shapeModelView struct {
	shape     model.Shape
	manager   *manager.ShapeManager
	observers []ShapeModelViewObserver

	position  geometry.Point
	size      geometry.Bounds
	style     geometry.Style
	shapeType model.ShapeType

	isDragging bool
	isResizing bool
	deleted    bool

	dragStartPosition geometry.Point

	resizeStartPosition geometry.Point
	resizeStartBounds   geometry.Bounds
}

func NewShapeModelView(
	shape model.Shape,
	manager *manager.ShapeManager,
) ShapeModelView {
	mv := &shapeModelView{
		shape:     shape,
		manager:   manager,
		position:  shape.GetPosition(),
		size:      shape.GetBounds(),
		style:     shape.GetStyle(),
		shapeType: shape.GetShapeType(),
	}
	shape.AddObserver(mv)

	return mv
}

func (s *shapeModelView) SetDeleted(deleted bool) {
	s.deleted = deleted
	s.notify()
}

func (s *shapeModelView) IsSelected() bool {
	return s.manager.IsSelected(s)
}

func (s *shapeModelView) IsDeleted() bool {
	return s.deleted
}

func (s *shapeModelView) Select(withCtrl bool) {
	s.manager.AppendToSelection(s, withCtrl)
	s.notify()
}

func (s *shapeModelView) Drag(delta geometry.Vector) {
	s.manager.Drag(delta, s.isDragging)
}

func (s *shapeModelView) Resize(delta geometry.Vector, scale geometry.Scale) {
	//scale := geometry.Scale{
	//	ScaleX: bounds.Width / s.size.Width,
	//	ScaleY: bounds.Height / s.size.Height,
	//}
	s.manager.Resize(delta, scale, s.isResizing)
}

func (s *shapeModelView) GetBounds() geometry.Bounds {
	return s.size
}

func (s *shapeModelView) GetPosition() geometry.Point {
	return s.position
}

func (s *shapeModelView) GetStyle() geometry.Style {
	return s.style
}

func (s *shapeModelView) GetShapeType() model.ShapeType {
	return s.shapeType
}

func (s *shapeModelView) AddObserver(o ShapeModelViewObserver) {
	s.observers = append(s.observers, o)
}

func (s *shapeModelView) notify() {
	for _, o := range s.observers {
		o.Update()
	}
}
