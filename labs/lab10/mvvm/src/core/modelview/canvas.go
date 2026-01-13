package modelview

import (
	"image/color"
	model2 "vector-editor/src/core/model"
	"vector-editor/src/manager"
	"vector-editor/src/types"
)

type CanvasModelView interface {
	GetShape(id types.ShapeId) ShapeModelView

	ClearSelection()
	Delete()

	AddObserver(o CanvasModelViewObserver)
	GetBackgroundColor() color.Color
}

type canvasModelView struct {
	observers  []CanvasModelViewObserver
	canvas     model2.Canvas
	manager    manager.CanvasManager
	background color.Color
	shapes     map[types.ShapeId]ShapeModelView
}

func NewCanvasModelView(
	canvas model2.Canvas,
	canvasManager manager.CanvasManager,
) CanvasModelView {
	canvasMV := &canvasModelView{
		canvas:     canvas,
		manager:    canvasManager,
		shapes:     make(map[types.ShapeId]ShapeModelView),
		background: canvas.GetBackground(),
	}
	canvasMV.manager.RegisterCanvas(canvasMV)
	canvasMV.GetCanvas().AddObserver(canvasMV)
	return canvasMV
}

func (c *canvasModelView) OnShapesChanged(ids []types.ShapeId) {
	for _, id := range ids {
		shape := c.canvas.GetShape(id)

		if shape == nil {
			if _, ok := c.shapes[id]; ok {
				delete(c.shapes, id)
			}
			continue
		}

		if _, ok := c.shapes[id]; ok {
			continue
		}
		c.shapes[id] = c.newShapeMV(shape)
	}
	for _, o := range c.observers {
		o.OnShapesChanged(ids)
	}
}

func (c *canvasModelView) newShapeMV(shape model2.Shape) ShapeModelView {
	return NewShapeModelView(shape, c.manager.ShapeManager())
}

func (c *canvasModelView) OnBackgroundChanged() {
	c.background = c.canvas.GetBackground()
	for _, o := range c.observers {
		o.OnBackgroundChanged()
	}
}

func (c *canvasModelView) GetBackgroundColor() color.Color {
	return c.background
}

func (c *canvasModelView) GetCanvas() model2.Canvas {
	return c.canvas
}

func (c *canvasModelView) MarkDeleted(ids []types.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(true)
	}
}

func (c *canvasModelView) Restore(ids []types.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(false)
	}
}

func (c *canvasModelView) GetShape(id types.ShapeId) ShapeModelView {
	return c.shapes[id]
}

func (c *canvasModelView) ClearSelection() {
	c.manager.ClearSelection()
}

func (c *canvasModelView) Delete() {
	c.manager.Delete()
}

func (c *canvasModelView) AddObserver(o CanvasModelViewObserver) {
	c.observers = append(c.observers, o)
}
