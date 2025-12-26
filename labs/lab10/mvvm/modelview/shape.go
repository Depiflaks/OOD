package modelview

import (
	"vector-editor/appmodel"
	"vector-editor/geometry"
	"vector-editor/model"
)

type editableShape struct {
	owner *ShapeModelView
}

func (s *editableShape) GetShape() *model.Shape {
	return s.owner.shape
}

func (s *editableShape) Move(delta geometry.Vector) {
	pos := s.owner.dragStartPosition
	pos.X += delta.X
	pos.Y += delta.Y
	s.owner.position = pos
}

func (s *editableShape) Scale(delta geometry.Vector, scale geometry.Scale) {
	startPos := s.owner.resizeStartPosition
	startBounds := s.owner.resizeStartBounds
	newPos, newBounds := geometry.CalculateScale(delta, scale, startPos, startBounds)
	s.owner.UpdateRect(newPos, newBounds)
}

func (s *editableShape) StartDragging() {
	if !s.owner.isDragging {
		s.owner.isDragging = true
		s.owner.dragStartPosition = s.owner.position
		s.owner.manager.StartDragging()
	}
}

func (s *editableShape) StopDragging() {
	if s.owner.isDragging {
		s.owner.isDragging = false
		s.owner.manager.StopDragging()
	}
}

func (s *editableShape) StartResizing() {
	if !s.owner.isResizing {
		s.owner.isResizing = true
		s.owner.resizeStartPosition = s.owner.position
		s.owner.resizeStartBounds = s.owner.size
		s.owner.manager.StartResizing()
	}
}

func (s *editableShape) StopResizing() {
	if s.owner.isResizing {
		s.owner.isResizing = false
		s.owner.manager.StopResizing()
	}
}

type shapeObserver struct {
	owner *ShapeModelView
}

func (s *shapeObserver) UpdateRect(position geometry.Point, bounds geometry.Bounds) {
	s.owner.position = position
	s.owner.size = bounds
	s.owner.notifyRect()
}

func (s *shapeObserver) UpdateStyle(style geometry.Style) {
	s.owner.style = style
	s.owner.manager.SetStyle(style)
	s.owner.notifyStyle()
}

type ShapeModelView struct {
	editableShape
	shapeObserver

	shape     *model.Shape
	manager   *appmodel.ShapeManager
	observers []ShapeModelViewObserver

	position  geometry.Point
	size      geometry.Bounds
	style     geometry.Style
	shapeType model.ShapeType

	isDragging bool
	isResizing bool

	dragStartPosition geometry.Point

	resizeStartPosition geometry.Point
	resizeStartBounds   geometry.Bounds
}

func NewShapeModelView(
	shape *model.Shape,
	manager *appmodel.ShapeManager,
) *ShapeModelView {
	mv := &ShapeModelView{
		shape:   shape,
		manager: manager,
	}

	mv.editableShape = editableShape{owner: mv}
	mv.shapeObserver = shapeObserver{owner: mv}

	mv.position = shape.GetPosition()
	mv.size = shape.GetBounds()
	mv.style = shape.GetStyle()
	mv.shapeType = shape.GetShapeType()

	shape.AddObserver(mv)

	return mv
}

func (s *ShapeModelView) Events() appmodel.ViewEvents {
	return &s.editableShape
}

func (s *ShapeModelView) Select(withCtrl bool) {
	s.manager.AppendToSelection(s, withCtrl)
}

func (s *ShapeModelView) Drag(delta geometry.Vector) {
	s.manager.Drag(delta, s.isDragging)
}

func (s *ShapeModelView) Resize(delta geometry.Vector, bounds geometry.Bounds) {
	scale := geometry.Scale{
		ScaleX: bounds.Width / s.size.Width,
		ScaleY: bounds.Height / s.size.Height,
	}
	s.manager.Resize(delta, scale, s.isResizing)
}

func (s *ShapeModelView) GetBounds() geometry.Bounds {
	return s.size
}

func (s *ShapeModelView) GetPosition() geometry.Point {
	return s.position
}

func (s *ShapeModelView) GetStyle() geometry.Style {
	return s.style
}

func (s *ShapeModelView) GetShapeType() model.ShapeType {
	return s.shapeType
}

func (s *ShapeModelView) notifyRect() {
	for _, o := range s.observers {
		o.UpdateRect(s.GetPosition(), s.GetBounds())
	}
}

func (s *ShapeModelView) notifyStyle() {
	for _, o := range s.observers {
		o.UpdateStyle(s.GetStyle())
	}
}
