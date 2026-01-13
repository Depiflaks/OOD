package model

import (
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
	"vector-editor/src/types/geometry"
)

type ShapeObserver interface {
	UpdateRect(position geometry.Point, bounds geometry.Bounds)
	UpdateStyle(style draw.Style)
}

type Shape interface {
	UpdateRect(p geometry.Point, b geometry.Bounds)
	Move(v geometry.Vector)
	SetStyle(st draw.Style)

	AddObserver(o ShapeObserver)

	GetBounds() geometry.Bounds
	GetPosition() geometry.Point
	GetStyle() draw.Style
	GetShapeId() types.ShapeId
	GetShapeType() types.ShapeType

	Dispose()
}

type shape struct {
	id        types.ShapeId
	shapeType types.ShapeType
	position  geometry.Point
	size      geometry.Bounds
	style     draw.Style
	observers []ShapeObserver

	storage FileStorage
}

func NewShape(
	t types.ShapeType,
	id types.ShapeId,
	style draw.Style,
) Shape {
	if style.BackgroundImagePath != nil && t != types.Rect {
		panic("Приложение поддерживает только прямоугольные картинки")
	}
	storage := NewStorage()
	if style.BackgroundImagePath != nil {
		newPath := storage.store(*style.BackgroundImagePath)
		style.BackgroundImagePath = &newPath
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
	if s.style.BackgroundImagePath != nil {
		s.storage.delete(*s.style.BackgroundImagePath)
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

func (s *shape) SetStyle(st draw.Style) {
	if st.BackgroundImagePath != nil {
		if s.style.BackgroundImagePath != nil {
			s.storage.delete(*s.style.BackgroundImagePath)
		}
		newPath := s.storage.store(*st.BackgroundImagePath)
		s.style.BackgroundImagePath = &newPath
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

func (s *shape) GetStyle() draw.Style {
	return s.style
}

func (s *shape) GetShapeId() types.ShapeId {
	return s.id
}

func (s *shape) GetShapeType() types.ShapeType {
	return s.shapeType
}

func (s *shape) AddObserver(o ShapeObserver) {
	s.observers = append(s.observers, o)
}
