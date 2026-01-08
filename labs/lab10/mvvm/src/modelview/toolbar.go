package modelview

import (
	"vector-editor/src/geometry"
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type ToolbarModelView interface {
	NewTriangle(style geometry.Style)
	NewRectangle(style geometry.Style)
	NewEllipse(style geometry.Style)
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

func (t *toolbarModelView) NewTriangle(style geometry.Style) {
	t.manager.NewShape(model.Triangle, style)
}

func (t *toolbarModelView) NewRectangle(style geometry.Style) {
	t.manager.NewShape(model.Rect, style)
}

func (t *toolbarModelView) NewEllipse(style geometry.Style) {
	t.manager.NewShape(model.Ellipse, style)
}

func (t *toolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}
