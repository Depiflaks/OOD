package modelview

import (
	"vector-editor/appmodel"
	"vector-editor/graphics"
	"vector-editor/model"
)

type editableShape struct {
	owner *ShapeModelView
}

func (s *editableShape) GetShape() *model.Shape {
	return s.owner.shape
}

func (s *editableShape) Move(delta graphics.Vector) {
	pos := s.owner.dragStartPosition
	pos.X += delta.X
	pos.Y += delta.Y
	s.owner.position = pos
}

func (s *editableShape) Scale(delta graphics.Vector, bounds graphics.Bounds) {
	// TODO: здесь провести скалирование
}

type shapeObserver struct {
	owner *ShapeModelView
}

func (s *shapeObserver) UpdateRect(position graphics.Point, bounds graphics.Bounds) {
	s.owner.position = position
	s.owner.size = bounds
	s.owner.notifyRect()
}

func (s *shapeObserver) UpdateStyle(style graphics.Style) {
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

	position  graphics.Point
	size      graphics.Bounds
	style     graphics.Style
	shapeType model.ShapeType

	isDragging bool
	isResizing bool

	dragStartPosition graphics.Point

	resizeStartPosition graphics.Point
	resizeStartBounds   graphics.Bounds
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

func (s *ShapeModelView) StartDragging() {
	s.isDragging = true
	s.dragStartPosition = s.position
}

func (s *ShapeModelView) Drag(delta graphics.Vector) {
	s.manager.Drag(delta, s.isDragging)
}

func (s *ShapeModelView) StopDragging() {
	s.isDragging = false
}

func (s *ShapeModelView) StartResizing() {
	s.isResizing = true
	s.resizeStartPosition = s.position
	s.resizeStartBounds = s.size
}

func (s *ShapeModelView) Resize(delta graphics.Vector, bounds graphics.Bounds) {
	// TODO: добавить сюда флаг
	s.manager.Resize(delta, bounds)
}

func (s *ShapeModelView) StopResizing() {
	s.isResizing = false
}

func (s *ShapeModelView) notifyRect() {
	for _, o := range s.observers {
		o.UpdateRect()
	}
}

func (s *ShapeModelView) notifyStyle() {
	for _, o := range s.observers {
		o.UpdateStyle()
	}
}
