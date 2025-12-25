package history

import "vector-editor/model"

type Command interface {
	Execute()
	Unexecute()
}

type Disposable interface {
	Dispose()
}

type NewShapeCommand struct {
	canvas *model.Canvas
	id     model.ShapeId
	t      model.ShapeType
}

func NewNewShapeCommand(canvas *model.Canvas, t model.ShapeType) *NewShapeCommand {
	return &NewShapeCommand{canvas: canvas, t: t}
}

func (c *NewShapeCommand) Execute() {
	c.id = c.canvas.NewShape(c.t)
}

func (c *NewShapeCommand) Undo() {
	c.canvas.GetCanvas().DeleteShapes([]model.ShapeId{c.shape.GetShapeId()})
}

func (c *NewShapeCommand) Redo() {
	c.Execute()
}
