package modelview

import (
	"image/color"
	"vector-editor/src/draw"
	"vector-editor/src/manager"
	"vector-editor/src/types"
)

type ToolbarModelView interface {
	NewTriangle(style draw.Style)
	NewRectangle(style draw.Style)
	NewEllipse(style draw.Style)
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

func (t *toolbarModelView) NewTriangle(style draw.Style) {
	t.manager.NewShape(types.Triangle, style)
}

func (t *toolbarModelView) NewRectangle(style draw.Style) {
	t.manager.NewShape(types.Rect, style)
}

func (t *toolbarModelView) NewEllipse(style draw.Style) {
	t.manager.NewShape(types.Ellipse, style)
}

func (t *toolbarModelView) NewImage(path string) {
	style := draw.Style{
		BackgroundImagePath: &path,
	}
	t.manager.NewShape(types.Rect, style)
}

func (t *toolbarModelView) AddObserver(o ToolbarModelViewObserver) {
	t.observers = append(t.observers, o)
}

func (t *toolbarModelView) SetFillColor(c color.Color) {
	t.manager.SetStyle(draw.Style{
		Fill:   &c,
		Stroke: nil,
	})
}

func (t *toolbarModelView) SetBorderColor(c color.Color) {
	t.manager.SetStyle(draw.Style{
		Fill:   nil,
		Stroke: &c,
	})
}

func (t *toolbarModelView) OnSelectionChange(style draw.Style, count int) {
	for _, o := range t.observers {
		o.OnSelectionChange(style, count)
	}
}
