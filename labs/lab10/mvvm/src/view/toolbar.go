package view

import (
	"image/color"
	"vector-editor/src/geometry"
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
)

type ToolbarView interface {
	Object() fyne.CanvasObject
}

type toolbarView struct {
	obj fyne.CanvasObject
	mv  modelview.ToolbarModelView

	curStyle geometry.Style

	fillPreview   *canvas.Rectangle
	strokePreview *canvas.Rectangle
	btnImage      *widget.Button
}

func NewToolbarView(
	win fyne.Window,
	mv modelview.ToolbarModelView,
	files FileActions,
	colors ColorActions,
) ToolbarView {
	view := &toolbarView{
		mv: mv,
		curStyle: geometry.Style{
			Fill:   nil,
			Stroke: nil,
		},
	}

	btnRect := widget.NewButton("Rect", func() { mv.NewRectangle(view.curStyle) })
	btnEllipse := widget.NewButton("Ellipse", func() { mv.NewEllipse(view.curStyle) })
	btnTriangle := widget.NewButton("Triangle", func() { mv.NewTriangle(view.curStyle) })

	view.fillPreview = canvas.NewRectangle(color.Transparent)
	view.fillPreview.SetMinSize(fyne.NewSize(24, 24))

	view.strokePreview = canvas.NewRectangle(color.Transparent)
	view.strokePreview.SetMinSize(fyne.NewSize(24, 24))

	btnFill := widget.NewButton("Fill", func() {
		d := dialog.NewColorPicker("Fill", "Select fill color", func(c color.Color) {
			var cc color.Color = c
			view.curStyle.Fill = &cc

			view.fillPreview.FillColor = c
			view.fillPreview.Refresh()

			if colors.SetFill != nil {
				colors.SetFill(c)
			}
		}, win)
		d.Show()
	})

	btnStroke := widget.NewButton("Stroke", func() {
		d := dialog.NewColorPicker("Stroke", "Select stroke color", func(c color.Color) {
			var cc = c
			view.curStyle.Stroke = &cc

			view.strokePreview.FillColor = c
			view.strokePreview.Refresh()

			if colors.SetStroke != nil {
				colors.SetStroke(c)
			}
		}, win)
		d.Show()
	})

	view.btnImage = widget.NewButton("Load Image", func() {
		// TODO: import image
	})
	view.btnImage.Hide()

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

	view.obj = container.NewHBox(
		btnRect, btnEllipse, btnTriangle,
		widget.NewSeparator(),
		container.NewHBox(btnFill, view.fillPreview),
		container.NewHBox(btnStroke, view.strokePreview),
		widget.NewSeparator(),
		view.btnImage,
		widget.NewSeparator(),
		btnOpen, btnSave, btnSaveAs,
	)

	mv.AddObserver(view)
	return view
}

func (t *toolbarView) Object() fyne.CanvasObject {
	return t.obj
}

func (t *toolbarView) OnSelectionChange(style geometry.Style, selectedCount int) {
	// 2) обновляем текущие значения инпутов и превью из стиля выделения
	t.curStyle = style

	// fill
	if style.Fill == nil {
		t.fillPreview.FillColor = color.Transparent
	} else {
		t.fillPreview.FillColor = *style.Fill
	}
	t.fillPreview.Refresh()

	if style.Stroke == nil {
		t.strokePreview.FillColor = color.Transparent
	} else {
		t.strokePreview.FillColor = *style.Stroke
	}
	t.strokePreview.Refresh()

	if selectedCount == 1 {
		t.btnImage.Show()
	} else {
		t.btnImage.Hide()
	}
	t.btnImage.Refresh()
}
