package model

import "vector-editor/geometry"

type ShapeType int

const (
	Rect ShapeType = iota
	Ellipse
	Triangle
)

type ShapeId int64

type ShapeObserver interface {
	UpdateRect(position geometry.Point, bounds geometry.Bounds)
	UpdateStyle(style geometry.Style)
}

type Shape struct {
	id        ShapeId
	shapeType ShapeType
	position  geometry.Point
	size      geometry.Bounds
	style     geometry.Style
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

func WithPosition(p geometry.Point) ShapeOption {
	return func(s *Shape) {
		s.position = p
	}
}

func WithBounds(b geometry.Bounds) ShapeOption {
	return func(s *Shape) {
		s.size = b
	}
}

func WithStyle(st geometry.Style) ShapeOption {
	return func(s *Shape) {
		s.SetStyle(st)
	}
}

func (s *Shape) UpdateRect(v geometry.Vector, b geometry.Bounds) {
	s.position.X += v.X
	s.position.Y += v.Y
	s.size = b
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *Shape) Move(v geometry.Vector) {
	s.position.X += v.X
	s.position.Y += v.Y
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *Shape) SetStyle(st geometry.Style) {
	if st.Fill != nil {
		s.style.Fill = st.Fill
	}
	if st.Stroke != nil {
		s.style.Stroke = st.Stroke
	}
	for _, o := range s.observers {
		o.UpdateStyle(st)
	}
}

func (s *Shape) GetBounds() geometry.Bounds {
	return s.size
}

func (s *Shape) GetPosition() geometry.Point {
	return s.position
}

func (s *Shape) GetStyle() geometry.Style {
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
