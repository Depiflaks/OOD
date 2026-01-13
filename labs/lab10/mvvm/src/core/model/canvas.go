package model

import (
	"fmt"
	"image/color"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
)

type CanvasObserver interface {
	OnShapesChanged(ids []types.ShapeId)
	OnBackgroundChanged()
}

type Canvas interface {
	NewShape(t types.ShapeType, style draw.Style) types.ShapeId
	ImportImage()
	GetShape(id types.ShapeId) Shape
	Shapes() map[types.ShapeId]Shape
	DeleteShapes(ids []types.ShapeId)
	AddObserver(o CanvasObserver)

	SetBackground(color color.Color)
	GetBackground() color.Color
}

type canvas struct {
	background color.Color
	shapes     map[types.ShapeId]Shape
	observers  []CanvasObserver
	nextId     types.ShapeId
}

func NewCanvas() Canvas {
	return &canvas{
		background: color.RGBA{R: 255, G: 255, B: 255, A: 255},
		shapes:     make(map[types.ShapeId]Shape),
	}
}

func (c *canvas) Shapes() map[types.ShapeId]Shape {
	return c.shapes
}

func (c *canvas) NewShape(t types.ShapeType, style draw.Style) types.ShapeId {
	defer func() {
		c.nextId += 1
	}()
	shape := NewShape(t, c.nextId, style)
	c.shapes[shape.GetShapeId()] = shape
	c.notifyShapesChanged([]types.ShapeId{shape.GetShapeId()})
	return c.nextId
}

func (c *canvas) ImportImage() {
	// TODO: implement me
}

func (c *canvas) GetShape(id types.ShapeId) Shape {
	return c.shapes[id]
}

func (c *canvas) DeleteShapes(ids []types.ShapeId) {
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

func (c *canvas) notifyShapesChanged(ids []types.ShapeId) {
	for _, o := range c.observers {
		o.OnShapesChanged(ids)
	}
}

func (c *canvas) notifyBackgroundChanged() {
	for _, o := range c.observers {
		o.OnBackgroundChanged()
	}
}
