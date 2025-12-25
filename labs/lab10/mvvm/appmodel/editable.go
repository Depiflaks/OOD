package appmodel

import (
	"image/color"
	"vector-editor/graphics"
	"vector-editor/model"
)

type ShapeId int64

type EditableShape interface {
	GetShapeId() ShapeId
	Move(delta graphics.Vector)
	Scale(delta graphics.Vector, bounds graphics.Bounds)
}

type EditableCanvas interface {
	SetBackground(color color.Color)
	GetCanvas() *model.Canvas
	MarkDeleted([]model.ShapeId)
}
