package model

import (
	"image/color"
)

type CanvasObserver interface {
	UpdateShapes(ids []ShapeId)
}

type Canvas struct {
	background color.Color
	shapes     map[ShapeId]*Shape
	observers  []*CanvasObserver
	nextId     ShapeId
}

func NewCanvas() *Canvas {
	return &Canvas{
		background: color.RGBA{R: 255, G: 255, B: 255, A: 255},
		shapes:     make(map[ShapeId]*Shape),
	}
}

func (c *Canvas) NewShape(t ShapeType) *Shape {
	c.nextId++
	shape := NewShape(t, c.nextId)
	c.shapes[shape.id] = shape
	c.notify([]ShapeId{shape.id})
	return shape
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

func (c *Canvas) AddObserver(o *CanvasObserver) {
	c.observers = append(c.observers, o)
}

func (c *Canvas) notify(ids []ShapeId) {
	for _, o := range c.observers {
		(*o).UpdateShapes(ids)
	}
}
