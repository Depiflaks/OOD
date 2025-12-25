package history

import (
	"vector-editor/graphics"
	"vector-editor/model"
)

type Command interface {
	Execute()
	Unexecute()
}

type Disposable interface {
	Dispose()
}

type CreateShapeFn func() model.ShapeId
type MarkDeleteShapesFn func(ids []model.ShapeId)
type RestoreShapesFn func(ids []model.ShapeId)
type DeleteShapesFn func(ids []model.ShapeId)

type MoveShapesFn func(delta graphics.Vector)
type ResizeShapesFn func(delta graphics.Vector, bounds graphics.Bounds)

type NewShapeCommand struct {
	create     CreateShapeFn
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	id model.ShapeId
}

func NewNewShapeCommand(
	create CreateShapeFn,
	markDelete MarkDeleteShapesFn,
	restore RestoreShapesFn,
	delete DeleteShapesFn,
) *NewShapeCommand {
	return &NewShapeCommand{
		create:     create,
		markDelete: markDelete,
		restore:    restore,
		delete:     delete,
	}
}

func (c *NewShapeCommand) Execute() {
	c.id = c.create()
}

func (c *NewShapeCommand) Unexecute() {
	c.markDelete([]model.ShapeId{c.id})
}

func (c *NewShapeCommand) Dispose() {
	c.delete([]model.ShapeId{c.id})
}

type DeleteShapeCommand struct {
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	ids []model.ShapeId
}

func NewDeleteShapeCommand(
	ids []model.ShapeId,
	markDelete MarkDeleteShapesFn,
	restore RestoreShapesFn,
	delete DeleteShapesFn,
) *DeleteShapeCommand {
	return &DeleteShapeCommand{
		ids:        ids,
		markDelete: markDelete,
		restore:    restore,
		delete:     delete,
	}
}

func (c *DeleteShapeCommand) Execute() {
	c.markDelete(c.ids)
}

func (c *DeleteShapeCommand) Unexecute() {
	c.restore(c.ids)
}

func (c *DeleteShapeCommand) Dispose() {
	c.delete(c.ids)
}

type MoveShapesCommand struct {
	move  MoveShapesFn
	delta graphics.Vector
}

func NewMoveShapesCommand(
	move MoveShapesFn,
	delta graphics.Vector,
) *MoveShapesCommand {
	return &MoveShapesCommand{
		move:  move,
		delta: delta,
	}
}

func (c *MoveShapesCommand) Execute() {
	c.move(c.delta)
}

func (c *MoveShapesCommand) Unexecute() {
	c.move(graphics.Vector{
		X: -c.delta.X,
		Y: -c.delta.Y,
	})
}

type ResizeShapesCommand struct {
	resize ResizeShapesFn
	delta  graphics.Vector
	bounds graphics.Bounds
}

func NewResizeShapesCommand(
	resize ResizeShapesFn,
	delta graphics.Vector,
	bounds graphics.Bounds,
) *ResizeShapesCommand {
	return &ResizeShapesCommand{
		resize: resize,
		delta:  delta,
		bounds: bounds,
	}
}

func (c *ResizeShapesCommand) Execute() {
	c.resize(c.delta, c.bounds)
}

func (c *ResizeShapesCommand) Unexecute() {
	c.resize(
		graphics.Vector{
			X: -c.delta.X,
			Y: -c.delta.Y,
		},
		c.bounds,
	)
}
