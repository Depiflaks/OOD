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
type SetStyleFn func(styles map[model.ShapeId]graphics.Style)

type NewShapeCommand struct {
	create     CreateShapeFn
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	id         model.ShapeId
	isExecuted bool
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
	c.isExecuted = true
}

func (c *NewShapeCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.markDelete([]model.ShapeId{c.id})
}

func (c *NewShapeCommand) Dispose() {
	if !c.isExecuted {
		return
	}
	c.delete([]model.ShapeId{c.id})
}

type DeleteShapeCommand struct {
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	ids        []model.ShapeId
	isExecuted bool
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
	c.isExecuted = true
}

func (c *DeleteShapeCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.restore(c.ids)
}

func (c *DeleteShapeCommand) Dispose() {
	if !c.isExecuted {
		return
	}
	c.delete(c.ids)
}

type MoveShapesCommand struct {
	move       MoveShapesFn
	delta      graphics.Vector
	isExecuted bool
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
	c.isExecuted = true
}

func (c *MoveShapesCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.move(graphics.Vector{
		X: -c.delta.X,
		Y: -c.delta.Y,
	})
}

type ResizeShapesCommand struct {
	resize     ResizeShapesFn
	delta      graphics.Vector
	bounds     graphics.Bounds
	isExecuted bool
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
	c.isExecuted = true
}

func (c *ResizeShapesCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.resize(
		graphics.Vector{
			X: -c.delta.X,
			Y: -c.delta.Y,
		},
		c.bounds,
	)
}

type SetStyleCommand struct {
	setStyle   SetStyleFn
	newStyles  map[model.ShapeId]graphics.Style
	prevStyles map[model.ShapeId]graphics.Style
	isExecuted bool
}

func NewSetStyleCommand(
	setStyle SetStyleFn,
	prevStyles map[model.ShapeId]graphics.Style,
	newStyles map[model.ShapeId]graphics.Style,
) *SetStyleCommand {
	return &SetStyleCommand{
		setStyle:   setStyle,
		prevStyles: prevStyles,
		newStyles:  newStyles,
	}
}

func (c *SetStyleCommand) Execute() {
	c.setStyle(c.newStyles)
	c.isExecuted = true
}

func (c *SetStyleCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.setStyle(c.prevStyles)
}
