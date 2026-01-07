package modelview

import (
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type ToolbarModelView struct {
	manager   manager.CanvasManager
	observers []ToolbarModelViewObserver
}

func NewToolbarModelView(
	canvasManager manager.CanvasManager,
) *ToolbarModelView {
	return &ToolbarModelView{
		manager: canvasManager,
	}
}

func (t *ToolbarModelView) NewTriangle() {
	t.manager.NewShape(model.Triangle)
}

func (t *ToolbarModelView) NewRectangle() {
	t.manager.NewShape(model.Rect)
}

func (t *ToolbarModelView) NewEllipse() {
	t.manager.NewShape(model.Ellipse)
}

func (t *ToolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}
