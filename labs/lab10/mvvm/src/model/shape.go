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

	Dispose()
}

type shape struct {
	id        ShapeId
	shapeType ShapeType
	position  geometry.Point
	size      geometry.Bounds
	style     geometry.Style
	observers []ShapeObserver

	storage FileStorage
}

func NewShape(
	t ShapeType,
	id ShapeId,
	style geometry.Style,
) Shape {
	if style.Image != nil && t != Rect {
		panic("Приложение поддерживает только прямоугольные картинки")
	}
	storage := NewStorage()
	fmt.Println("here")
	if style.Image != nil {
		newPath := storage.store(*style.Image)
		fmt.Println(newPath)
		style.Image = &newPath
	}
	return &shape{
		id:        id,
		shapeType: t,
		position:  geometry.Point{X: 100, Y: 100},
		size:      geometry.Bounds{Width: 60, Height: 120},
		storage:   storage,
		style:     style,
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

func (s *shape) Dispose() {
	if s.style.Image != nil {
		s.storage.delete(*s.style.Image)
	}
}

func (s *shape) UpdateRect(p geometry.Point, b geometry.Bounds) {
	if p.X == s.position.X && p.Y == s.position.Y &&
		b.Width == s.size.Width && b.Height == s.size.Height {
		return
	}
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
	if st.Image != nil {
		if s.style.Image != nil {
			s.storage.delete(*s.style.Image)
		}
		newPath := s.storage.store(*st.Image)
		s.style.Image = &newPath
		// TODO: предусмотреть замену изображений
		panic("Пока что замена изображений не предусмотрена")
	}
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
