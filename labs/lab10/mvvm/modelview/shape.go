package modelview

import (
	"vector-editor/appmodel"
	"vector-editor/graphics"
	"vector-editor/model"
)

type editableShape struct {
}

func (s *editableShape) GetShape() *model.Shape {
	//TODO implement me
	panic("implement me")
}

func (s *editableShape) Move(delta graphics.Vector) {
	//TODO implement me
	panic("implement me")
}

func (s *editableShape) Scale(delta graphics.Vector, bounds graphics.Bounds) {
	//TODO implement me
	panic("implement me")
}

type shapeObserver struct{}

func (s shapeObserver) UpdateRect(position graphics.Vector, bounds graphics.Bounds) {
	//TODO implement me
	panic("implement me")
}

func (s shapeObserver) UpdateStyle(style graphics.Style) {
	//TODO implement me
	panic("implement me")
}

type ShapeModelView struct {
	editableShape
	shapeObserver

	shape     *model.Shape
	manager   *appmodel.ShapeManager
	observers []ShapeModelViewObserver

	position  graphics.Vector
	size      graphics.Bounds
	style     graphics.Style
	shapeType model.ShapeType

	isDragging bool
	isResizing bool
}

func NewShapeModelView(
	shape *model.Shape,
	manager *appmodel.ShapeManager,
) *ShapeModelView {
	return &ShapeModelView{
		shape:   shape,
		manager: manager,
	}
}

func (s *ShapeModelView) GetPosition() graphics.Vector {
	return s.position
}

func (s *ShapeModelView) GetBounds() graphics.Bounds {
	return s.size
}

func (s *ShapeModelView) GetStyle() graphics.Style {
	return s.style
}

func (s *ShapeModelView) GetShapeType() model.ShapeType {
	return s.shapeType
}

func (s *ShapeModelView) SetStyle(style graphics.Style) {
	s.manager.
		s.style = style
	s.notifyStyle()
}

func (s *ShapeModelView) Select(withCtrl bool) {
	s.manager.AppendToSelection(s, withCtrl)
}

func (s *ShapeModelView) IsSelected() bool {
	return s.manager.IsSelected(s)
}

func (s *ShapeModelView) StartDragging() {
	s.isDragging = true
}

func (s *ShapeModelView) Drag(delta graphics.Vector) {
	s.manager.Drag(delta, s.isDragging)
}

func (s *ShapeModelView) StopDragging() {
	s.isDragging = false
}

func (s *ShapeModelView) StartResizing() {
	s.isResizing = true
}

func (s *ShapeModelView) Resize(delta graphics.Vector, bounds graphics.Bounds) {
	s.manager.Resize(delta, bounds)
}

func (s *ShapeModelView) StopResizing() {
	s.isResizing = false
}

func (s *ShapeModelView) AddObserver(o ShapeModelViewObserver) {
	s.observers = append(s.observers, o)
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
