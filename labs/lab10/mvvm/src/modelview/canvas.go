package modelview

import (
	"image/color"
	"vector-editor/src/appmodel"
	"vector-editor/src/model"
)

type CanvasModelView struct {
	observers []CanvasModelViewObserver
	canvas    *model.Canvas
	manager   *appmodel.CanvasManager
	shapes    map[model.ShapeId]*ShapeModelView
}

func NewCanvasModelView(
	canvas *model.Canvas,
	canvasManager *appmodel.CanvasManager,
) *CanvasModelView {
	return &CanvasModelView{
		canvas:  canvas,
		manager: canvasManager,
		shapes:  make(map[model.ShapeId]*ShapeModelView),
	}
}

func (c *CanvasModelView) UpdateShapes(ids []model.ShapeId) {
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

func (c *CanvasModelView) newShapeMV(shape *model.Shape) *ShapeModelView {
	return NewShapeModelView(shape, c.manager.ShapeManager())
}

func (c *CanvasModelView) SetBackground(color color.Color) {
	// TODO: метод изменения фона
}

func (c *CanvasModelView) GetCanvas() *model.Canvas {
	return c.canvas
}

func (c *CanvasModelView) MarkDeleted(ids []model.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(true)
	}
}

func (c *CanvasModelView) Restore(ids []model.ShapeId) {
	for _, id := range ids {
		c.shapes[id].SetDeleted(false)
	}
}

func (c *CanvasModelView) GetShape(id model.ShapeId) *ShapeModelView {
	return c.shapes[id]
}

func (c *CanvasModelView) ClearSelection() {
	c.manager.ClearSelection()
}

func (c *CanvasModelView) Delete() {
	c.manager.Delete()
}

func (c *CanvasModelView) AddObserver(o CanvasModelViewObserver) {
	c.observers = append(c.observers, o)
}
