package model

import (
	"image/color"
	"vector-editor/src/geometry"
)

type CanvasObserver interface {
	UpdateShapes(ids []ShapeId)
}

type Canvas struct {
	background color.Color
	shapes     map[ShapeId]*Shape
	observers  []CanvasObserver
	nextId     ShapeId
}

func NewCanvas() *Canvas {
	return &Canvas{
		background: color.RGBA{R: 255, G: 255, B: 255, A: 255},
		shapes:     make(map[ShapeId]*Shape),
	}
}

func (c *Canvas) NewShape(t ShapeType) ShapeId {
	defer func() {
		c.nextId += 1
	}()
	shape := NewShape(t, c.nextId)
	var col color.Color = color.RGBA{R: 255, G: 3, B: 34, A: 1}
	shape.SetStyle(geometry.Style{
		Fill:   &col,
		Stroke: nil,
	})
	c.shapes[shape.id] = shape
	c.notify([]ShapeId{shape.id})
	return c.nextId
}

func (c *Canvas) ImportImage() {
	// TODO: implement me
}

func (c *Canvas) GetShape(id ShapeId) *Shape {
	return c.shapes[id]
}

func (c *Canvas) DeleteShapes(ids []ShapeId) {
	for _, id := range ids {
		delete(c.shapes, id)
	}
	c.notify(ids)
}

func (c *Canvas) AddObserver(o CanvasObserver) {
	c.observers = append(c.observers, o)
}

func (c *Canvas) notify(ids []ShapeId) {
	for _, o := range c.observers {
		o.UpdateShapes(ids)
	}
}
