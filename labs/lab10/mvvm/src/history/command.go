package history

import (
	"image/color"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
	"vector-editor/src/types/geometry"
)

type Command interface {
	Execute()
	Unexecute()
}

type Disposable interface {
	Dispose()
}

<<<<<<< Updated upstream
type CreateShapeFn func() types.ShapeId
type MarkDeleteShapesFn func(ids []types.ShapeId)
type RestoreShapesFn func(ids []types.ShapeId)
type DeleteShapesFn func(ids []types.ShapeId)
=======
type IdType int16

type CreateFn func() IdType
type MarkDeleteFn func(ids []IdType)
type RestoreFn func(ids []IdType)
type DeleteFn func(ids []IdType)
>>>>>>> Stashed changes
type SetBackgroundColorFn func(color color.Color)

type MoveShapesFn func(delta geometry.Vector)
type ResizeShapesFn func(rects map[types.ShapeId]geometry.Rect)
type SetStyleFn func(styles map[types.ShapeId]draw.Style)

type NewShapeCommand struct {
	create     CreateShapeFn
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	id                  types.ShapeId
	isExecuted          bool
	isExecutedFirstTime bool
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
	if c.isExecutedFirstTime {
		c.restore([]types.ShapeId{c.id})
	} else {
		c.id = c.create()
		c.isExecutedFirstTime = true
	}
	c.isExecuted = true
}

func (c *NewShapeCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.isExecuted = false
	c.markDelete([]types.ShapeId{c.id})
}

func (c *NewShapeCommand) Dispose() {
	if c.isExecuted {
		return
	}
	c.delete([]types.ShapeId{c.id})
}

type DeleteShapeCommand struct {
	markDelete MarkDeleteShapesFn
	restore    RestoreShapesFn
	delete     DeleteShapesFn

	ids        []types.ShapeId
	isExecuted bool
}

func NewDeleteShapeCommand(
	ids []types.ShapeId,
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
	c.isExecuted = false
	c.restore(c.ids)
}

func (c *DeleteShapeCommand) Dispose() {
	if c.isExecuted {
		c.delete(c.ids)
	}
}

type MoveShapesCommand struct {
	move       MoveShapesFn
	delta      geometry.Vector
	isExecuted bool
}

func NewMoveShapesCommand(
	move MoveShapesFn,
	delta geometry.Vector,
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
	c.move(geometry.Vector{
		X: -c.delta.X,
		Y: -c.delta.Y,
	})
}

type ResizeShapesCommand struct {
	resize     ResizeShapesFn
	newRects   map[types.ShapeId]geometry.Rect
	curRects   map[types.ShapeId]geometry.Rect
	isExecuted bool
}

func NewResizeShapesCommand(
	resize ResizeShapesFn,
	newRects map[types.ShapeId]geometry.Rect,
	curRects map[types.ShapeId]geometry.Rect,
) *ResizeShapesCommand {
	return &ResizeShapesCommand{
		resize:   resize,
		newRects: newRects,
		curRects: curRects,
	}
}

func (c *ResizeShapesCommand) Execute() {
	c.resize(c.newRects)

	c.isExecuted = true
}

func (c *ResizeShapesCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.resize(c.curRects)
}

type SetStyleCommand struct {
	setStyle   SetStyleFn
	newStyles  map[types.ShapeId]draw.Style
	prevStyles map[types.ShapeId]draw.Style
	isExecuted bool
}

func NewSetStyleCommand(
	setStyle SetStyleFn,
	prevStyles map[types.ShapeId]draw.Style,
	newStyles map[types.ShapeId]draw.Style,
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

type SetBackgroundCommand struct {
	setBackground SetBackgroundColorFn
	newColor      color.Color
	prevColor     color.Color
	isExecuted    bool
}

func NewSetBackgroundCommand(
	setBackground SetBackgroundColorFn,
	prevColor color.Color,
	newColor color.Color,
) *SetBackgroundCommand {
	return &SetBackgroundCommand{
		setBackground: setBackground,
		prevColor:     prevColor,
		newColor:      newColor,
	}
}

func (c *SetBackgroundCommand) Execute() {
	c.setBackground(c.newColor)
	c.isExecuted = true
}

func (c *SetBackgroundCommand) Unexecute() {
	if !c.isExecuted {
		return
	}
	c.setBackground(c.prevColor)
}
