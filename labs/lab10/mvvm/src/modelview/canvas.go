package modelview

import (
	"image/color"
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type CanvasModelView interface {
	GetShape(id model.ShapeId) ShapeModelView

	ClearSelection()
	Delete()

	AddObserver(o CanvasModelViewObserver)
}

type canvasModelView struct {
	observers []CanvasModelViewObserver
	canvas    *model.Canvas
	manager   manager.CanvasManager
	shapes    map[model.ShapeId]ShapeModelView
}

func NewCanvasModelView(
	canvas *model.Canvas,
	canvasManager manager.CanvasManager,
) CanvasModelView {
	canvasMV := &canvasModelView{
		canvas:  canvas,
		manager: canvasManager,
		shapes:  make(map[model.ShapeId]ShapeModelView),
	}
	canvasMV.manager.RegisterCanvas(canvasMV)
	canvasMV.GetCanvas().AddObserver(canvasMV)
	return canvasMV
}

func (c *canvasModelView) UpdateShapes(ids []model.ShapeId) {
	for _, id := range ids {
		if _, ok := c.shapes[id]; ok {
			continue
		}
		shape := c.canvas.GetShape(id)
		if shape == nil {
			continue
		}
		c.shapes[id] = c.newShapeMV(shape)
	}
	for _, o := range c.observers {
		o.UpdateShapes(ids)
	}
}

func (c *canvasModelView) newShapeMV(shape model.Shape) ShapeModelView {
	return NewShapeModelView(shape, c.manager.ShapeManager())
}

func (c *canvasModelView) SetBackground(color color.Color) {
	// TODO: метод изменения фона
}

func (c *canvasModelView) GetCanvas() *model.Canvas {
	return c.canvas
}

func (c *canvasModelView) MarkDeleted(ids []model.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(true)
	}
}

func (c *canvasModelView) Restore(ids []model.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(false)
	}
}

func (c *canvasModelView) GetShape(id model.ShapeId) ShapeModelView {
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
