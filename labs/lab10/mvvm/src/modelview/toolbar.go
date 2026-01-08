package modelview

import (
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type ToolbarModelView interface {
	NewTriangle()
	NewRectangle()
	NewEllipse()
	AddObserver(o ToolbarModelViewObserver)
}

type toolbarModelView struct {
	manager   manager.CanvasManager
	observers []ToolbarModelViewObserver
}

func NewToolbarModelView(
	canvasManager manager.CanvasManager,
) ToolbarModelView {
	return &toolbarModelView{
		manager: canvasManager,
	}
}

func (t *toolbarModelView) NewTriangle() {
	t.manager.NewShape(model.Triangle)
}

func (t *toolbarModelView) NewRectangle() {
	t.manager.NewShape(model.Rect)
}

func (t *toolbarModelView) NewEllipse() {
	t.manager.NewShape(model.Ellipse)
}

func (t *toolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}
