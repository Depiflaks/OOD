package view

import (
	"image/color"
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
)

type ToolbarView struct {
	obj fyne.CanvasObject
}

func NewToolbarView(
	win fyne.Window,
	mv *modelview.ToolbarModelView,
	files FileActions,
	colors ColorActions,
) *ToolbarView {
	btnRect := widget.NewButton("Rect", func() { mv.NewRectangle() })
	btnEllipse := widget.NewButton("Ellipse", func() { mv.NewEllipse() })
	btnTriangle := widget.NewButton("Triangle", func() { mv.NewTriangle() })

	fillPreview := canvas.NewRectangle(color.Transparent)
	fillPreview.SetMinSize(fyne.NewSize(24, 24))

	strokePreview := canvas.NewRectangle(color.Transparent)
	strokePreview.SetMinSize(fyne.NewSize(24, 24))

	btnFill := widget.NewButton("Fill", func() {
		d := dialog.NewColorPicker("Fill", "Select fill color", func(c color.Color) {
			fillPreview.FillColor = c
			fillPreview.Refresh()
			if colors.SetFill != nil {
				colors.SetFill(c)
			}
		}, win)
		d.Show()
	})

	btnStroke := widget.NewButton("Stroke", func() {
		d := dialog.NewColorPicker("Stroke", "Select stroke color", func(c color.Color) {
			strokePreview.FillColor = c
			strokePreview.Refresh()
			if colors.SetStroke != nil {
				colors.SetStroke(c)
			}
		}, win)
		d.Show()
	})

	btnOpen := widget.NewButton("Open", func() {
		if files.Open != nil {
			files.Open()
		}
	})
	btnSave := widget.NewButton("Save", func() {
		if files.Save != nil {
			files.Save()
		}
	})
	btnSaveAs := widget.NewButton("Save As", func() {
		if files.SaveAs != nil {
			files.SaveAs()
		}
	})

	obj := container.NewHBox(
		btnRect, btnEllipse, btnTriangle,
		widget.NewSeparator(),
		container.NewHBox(btnFill, fillPreview),
		container.NewHBox(btnStroke, strokePreview),
		widget.NewSeparator(),
		btnOpen, btnSave, btnSaveAs,
	)

	return &ToolbarView{obj: obj}
}

func (t *ToolbarView) Object() fyne.CanvasObject {
	return t.obj
}
