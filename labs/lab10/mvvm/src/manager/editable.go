package manager

import (
	model2 "vector-editor/src/core/model"
	"vector-editor/src/geometry"
	"vector-editor/src/types"
)

type EditableShape interface {
	ViewEvents

	GetShape() model2.Shape
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
	GetCanvas() model2.Canvas
	MarkDeleted([]types.ShapeId)
	Restore([]types.ShapeId)
}
