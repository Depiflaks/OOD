package model

import (
	"fmt"
	"image/color"
	"vector-editor/src/geometry"
)

type CanvasObserver interface {
	OnShapesChanged(ids []ShapeId)
	OnBackgroundChanged()
}

type Canvas interface {
	NewShape(t ShapeType, style geometry.Style) ShapeId
	ImportImage()
	GetShape(id ShapeId) Shape
	Shapes() map[ShapeId]Shape
	DeleteShapes(ids []ShapeId)
	AddObserver(o CanvasObserver)

	SetBackground(color color.Color)
	GetBackground() color.Color
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

func (c *canvas) NewShape(t ShapeType, style geometry.Style) ShapeId {
	defer func() {
		c.nextId += 1
	}()
	shape := NewShape(t, c.nextId, style)
	c.shapes[shape.GetShapeId()] = shape
	c.notifyShapesChanged([]ShapeId{shape.GetShapeId()})
	return c.nextId
}

func (c *canvas) ImportImage() {
	// TODO: implement me
}

func (c *canvas) GetShape(id ShapeId) Shape {
	return c.shapes[id]
}

func (c *canvas) DeleteShapes(ids []ShapeId) {
	fmt.Println("real delete")
	for _, id := range ids {
		c.shapes[id].Dispose()
		delete(c.shapes, id)
	}
	c.notifyShapesChanged(ids)
}

func (c *canvas) AddObserver(o CanvasObserver) {
	c.observers = append(c.observers, o)
}

func (c *canvas) SetBackground(color color.Color) {
	c.background = color
	c.notifyBackgroundChanged()
}

func (c *canvas) GetBackground() color.Color {
	return c.background
}

func (c *canvas) notifyShapesChanged(ids []ShapeId) {
	for _, o := range c.observers {
		o.OnShapesChanged(ids)
	}
}

func (c *canvas) notifyBackgroundChanged() {
	for _, o := range c.observers {
		o.OnBackgroundChanged()
	}
}
