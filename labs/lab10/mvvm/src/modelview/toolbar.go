package modelview

import (
	"vector-editor/src/appmodel"
	"vector-editor/src/model"
)

type ToolbarModelView struct {
	canvasManager *appmodel.CanvasManager
	observers     []ToolbarModelViewObserver
}

func NewToolbarModelView(
	canvasManager *appmodel.CanvasManager,
) *ToolbarModelView {
	return &ToolbarModelView{
		canvasManager: canvasManager,
	}
}

func (t *ToolbarModelView) NewTriangle() {
	t.canvasManager.NewShape(model.Triangle)
}

func (t *ToolbarModelView) NewRectangle() {
	t.canvasManager.NewShape(model.Rect)
}

func (t *ToolbarModelView) NewEllipse() {
	t.canvasManager.NewShape(model.Ellipse)
}

func (t *ToolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}
