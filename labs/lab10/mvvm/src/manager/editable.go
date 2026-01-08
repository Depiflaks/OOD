package manager

import (
	"vector-editor/src/geometry"
	"vector-editor/src/model"
)

type EditableShape interface {
	ViewEvents

	GetShape() model.Shape
	Move(delta geometry.Vector)
	Scale(delta geometry.Vector, scale geometry.Scale)
	Notify()
}

type ViewEvents interface {
	StartDragging()
	StopDragging()
	StartResizing()
	StopResizing()
}

type EditableCanvas interface {
	GetCanvas() model.Canvas
	MarkDeleted([]model.ShapeId)
	Restore([]model.ShapeId)
}
