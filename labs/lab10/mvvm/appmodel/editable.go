package appmodel

import (
	"image/color"
	"vector-editor/graphics"
	"vector-editor/model"
)

// TODO: разобраться с перемещениями объектов
type EditableShape interface {
	GetShape() *model.Shape
	Move(delta graphics.Vector)
	Scale(delta graphics.Vector, bounds graphics.Bounds)
}

type EditableCanvas interface {
	SetBackground(color color.Color)
	GetCanvas() *model.Canvas
	MarkDeleted([]model.ShapeId)
	Restore([]model.ShapeId)
}
