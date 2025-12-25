package appmodel

import (
	"image/color"
	"vector-editor/graphics"
	"vector-editor/model"
)

// TODO: разобраться с перемещениями объектов
type EditableShape interface {
	ViewEvents

	GetShape() *model.Shape
	Move(delta graphics.Vector)
	Scale(delta graphics.Vector, scale graphics.Scale)
}

type ViewEvents interface {
	StartDragging()
	StopDragging()
	StartResizing()
	StopResizing()
}

type EditableCanvas interface {
	SetBackground(color color.Color)
	GetCanvas() *model.Canvas
	MarkDeleted([]model.ShapeId)
	Restore([]model.ShapeId)
}
