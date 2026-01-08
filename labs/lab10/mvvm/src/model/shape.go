package model

import (
	"fmt"
	"vector-editor/src/geometry"
)

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

type Shape interface {
	UpdateRect(p geometry.Point, b geometry.Bounds)
	Move(v geometry.Vector)
	SetStyle(st geometry.Style)
	AddObserver(o ShapeObserver)

	GetBounds() geometry.Bounds
	GetPosition() geometry.Point
	GetStyle() geometry.Style
	GetShapeId() ShapeId
	GetShapeType() ShapeType
}

type shape struct {
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
) Shape {
	return &shape{
		id:        id,
		shapeType: t,
		position:  geometry.Point{X: 100, Y: 100},
		size:      geometry.Bounds{Width: 60, Height: 120},
	}
}

type ShapeOption func(*shape)

func WithPosition(p geometry.Point) ShapeOption {
	return func(s *shape) {
		s.position = p
	}
}

func WithBounds(b geometry.Bounds) ShapeOption {
	return func(s *shape) {
		s.size = b
	}
}

func WithStyle(st geometry.Style) ShapeOption {
	return func(s *shape) {
		s.SetStyle(st)
	}
}

func (s *shape) UpdateRect(p geometry.Point, b geometry.Bounds) {
	if p.X == s.position.X && p.Y == s.position.Y &&
		b.Width == s.size.Width && b.Height == s.size.Height {
		return
	}
	fmt.Println("model rect update", s.id)
	s.position = p
	s.size = b
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *shape) Move(v geometry.Vector) {
	if v.X == 0 && v.Y == 0 {
		return
	}
	s.position.X += v.X
	s.position.Y += v.Y
	for _, o := range s.observers {
		o.UpdateRect(s.position, s.size)
	}
}

func (s *shape) SetStyle(st geometry.Style) {
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

func (s *shape) GetBounds() geometry.Bounds {
	return s.size
}

func (s *shape) GetPosition() geometry.Point {
	return s.position
}

func (s *shape) GetStyle() geometry.Style {
	return s.style
}

func (s *shape) GetShapeId() ShapeId {
	return s.id
}

func (s *shape) GetShapeType() ShapeType {
	return s.shapeType
}

func (s *shape) AddObserver(o ShapeObserver) {
	s.observers = append(s.observers, o)
}
