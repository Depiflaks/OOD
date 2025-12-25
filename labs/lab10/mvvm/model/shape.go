package model

import "vector-editor/graphics"

type ShapeType int

const (
	Rect ShapeType = iota
	Ellipse
	Triangle
)

type ShapeId int64

type ShapeObserver interface {
	UpdateRect(position graphics.Vector, bounds graphics.Bounds)
	UpdateStyle(style graphics.Style)
}

type Shape struct {
	id        ShapeId
	shapeType ShapeType
	position  graphics.Vector
	size      graphics.Bounds
	style     graphics.Style
	observers []ShapeObserver
}

func NewShape(
	t ShapeType,
	id ShapeId,
) *Shape {
	return &Shape{
		id:        id,
		shapeType: t,
	}
}

type ShapeOption func(*Shape)

func WithPosition(p graphics.Vector) ShapeOption {
	return func(s *Shape) {
		s.position = p
	}
}

func WithBounds(b graphics.Bounds) ShapeOption {
	return func(s *Shape) {
		s.size = b
	}
}

func WithStyle(st graphics.Style) ShapeOption {
	return func(s *Shape) {
		s.style = st
	}
}

func (s *Shape) UpdateRect(v graphics.Vector, b graphics.Bounds) {
	s.position.X += v.X
	s.position.Y += v.Y
	s.size = b
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *Shape) Move(v graphics.Vector) {
	s.position.X += v.X
	s.position.Y += v.Y
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *Shape) Resize(b graphics.Bounds) {
	s.size = b
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *Shape) SetStyle(st graphics.Style) {
	s.style = st
	for _, o := range s.observers {
		o.UpdateStyle(st)
	}
}

func (s *Shape) GetBounds() graphics.Bounds {
	return s.size
}

func (s *Shape) GetPosition() graphics.Vector {
	return s.position
}

func (s *Shape) GetStyle() graphics.Style {
	return s.style
}

func (s *Shape) GetShapeId() ShapeId {
	return s.id
}

func (s *Shape) GetShapeType() ShapeType {
	return s.shapeType
}

func (s *Shape) AddObserver(o ShapeObserver) {
	s.observers = append(s.observers, o)
}
