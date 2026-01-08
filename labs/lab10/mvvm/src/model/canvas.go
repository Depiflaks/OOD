package model

import (
	"image/color"
	"vector-editor/src/geometry"
)

type CanvasObserver interface {
	OnShapesChanged(ids []ShapeId)
}

type Canvas interface {
	NewShape(t ShapeType) ShapeId
	ImportImage()
	GetShape(id ShapeId) Shape
	Shapes() map[ShapeId]Shape
	DeleteShapes(ids []ShapeId)
	AddObserver(o CanvasObserver)
}

type canvas struct {
	background color.Color
	shapes     map[ShapeId]Shape
	observers  []CanvasObserver
	nextId     ShapeId
}

func NewCanvas() Canvas {
	return &canvas{
		background: color.RGBA{R: 255, G: 255, B: 255, A: 255},
		shapes:     make(map[ShapeId]Shape),
	}
}

func (c *canvas) Shapes() map[ShapeId]Shape {
	return c.shapes
}

func (c *canvas) NewShape(t ShapeType) ShapeId {
	defer func() {
		c.nextId += 1
	}()
	shape := NewShape(t, c.nextId)
	var col color.Color = color.RGBA{R: 255, G: 3, B: 34, A: 1}
	shape.SetStyle(geometry.Style{
		Fill:   &col,
		Stroke: nil,
	})
	c.shapes[shape.GetShapeId()] = shape
	c.notify([]ShapeId{shape.GetShapeId()})
	return c.nextId
}

func (c *canvas) ImportImage() {
	// TODO: implement me
}

func (c *canvas) GetShape(id ShapeId) Shape {
	return c.shapes[id]
}

func (c *canvas) DeleteShapes(ids []ShapeId) {
	for _, id := range ids {
		delete(c.shapes, id)
	}
	c.notify(ids)
}

func (c *canvas) AddObserver(o CanvasObserver) {
	c.observers = append(c.observers, o)
}

func (c *canvas) notify(ids []ShapeId) {
	for _, o := range c.observers {
		o.OnShapesChanged(ids)
	}
}
