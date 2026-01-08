package modelview

import (
	"image/color"
	"vector-editor/src/geometry"
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type ToolbarModelView interface {
	NewTriangle(style geometry.Style)
	NewRectangle(style geometry.Style)
	NewEllipse(style geometry.Style)
	AddObserver(o ToolbarModelViewObserver)

	SetFillColor(c color.Color)
	SetBorderColor(c color.Color)
	NewImage(path string)
}

type toolbarModelView struct {
	manager   manager.CanvasManager
	observers []ToolbarModelViewObserver
}

func NewToolbarModelView(
	canvasManager manager.CanvasManager,
) ToolbarModelView {
	tmv := &toolbarModelView{
		manager: canvasManager,
	}
	canvasManager.ShapeManager().AddObserver(tmv)
	return tmv
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

func (t *toolbarModelView) NewImage(path string) {
	style := geometry.Style{
		BackgroundImagePath: &path,
	}
	t.manager.NewShape(model.Rect, style)
}

func (t *toolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}

func (t *toolbarModelView) SetFillColor(c color.Color) {
	t.manager.SetStyle(geometry.Style{
		Fill:   &c,
		Stroke: nil,
	})
}

func (t *toolbarModelView) SetBorderColor(c color.Color) {
	t.manager.SetStyle(geometry.Style{
		Fill:   nil,
		Stroke: &c,
	})
}

func (t *toolbarModelView) OnSelectionChange(style geometry.Style, count int) {
	for _, o := range t.observers {
		o.OnSelectionChange(style, count)
	}
}
