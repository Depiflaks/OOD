package appmodel

import (
	"image/color"
	"vector-editor/graphics"
	"vector-editor/model"
)

type ShapeId int64

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

// TODO: возможно, можно сделать приватным
func ConvertShapeIds(
	in []ShapeId,
) []model.ShapeId {
	out := make([]model.ShapeId, len(in))
	for i, id := range in {
		out[i] = model.ShapeId(id)
	}
	return out
}
