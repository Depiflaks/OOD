package manager

import (
	"image/color"
	"vector-editor/src/geometry"
	"vector-editor/src/model"
)

type EditableShape interface {
	ViewEvents

	GetShape() model.Shape
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
	GetCanvas() *model.Canvas
	MarkDeleted([]model.ShapeId)
	Restore([]model.ShapeId)
}
