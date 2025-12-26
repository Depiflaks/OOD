package appmodel

import (
	"image/color"
	"vector-editor/src/geometry"
	model2 "vector-editor/src/model"
)

// TODO: разобраться с перемещениями объектов
type EditableShape interface {
	ViewEvents

	GetShape() *model2.Shape
	Move(delta geometry.Vector)
	Scale(delta geometry.Vector, scale geometry.Scale)
}

type ViewEvents interface {
	StartDragging()
	StopDragging()
	StartResizing()
	StopResizing()
}

type EditableCanvas interface {
	SetBackground(color color.Color)
	GetCanvas() *model2.Canvas
	MarkDeleted([]model2.ShapeId)
	Restore([]model2.ShapeId)
}
