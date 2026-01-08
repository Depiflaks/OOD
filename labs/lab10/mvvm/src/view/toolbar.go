package view

import (
	"image/color"
	"vector-editor/src/geometry"
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/storage"
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

	defaultFill   color.RGBA
	defaultStroke color.RGBA
}

func NewToolbarView(
	win fyne.Window,
	mv modelview.ToolbarModelView,
	files FileActions,
) ToolbarView {
	defaultFill := color.RGBA{R: 255, G: 3, B: 34, A: 255}
	defaultStroke := color.RGBA{R: 0, G: 0, B: 0, A: 255}

	view := &toolbarView{
		mv: mv,
		curStyle: geometry.Style{
			Fill:   func() *color.Color { var c color.Color = defaultFill; return &c }(),
			Stroke: func() *color.Color { var c color.Color = defaultStroke; return &c }(),
		},
		defaultFill:   defaultFill,
		defaultStroke: defaultStroke,
	}

	btnRect := widget.NewButton("Rect", func() { mv.NewRectangle(view.curStyle) })
	btnEllipse := widget.NewButton("Ellipse", func() { mv.NewEllipse(view.curStyle) })
	btnTriangle := widget.NewButton("Triangle", func() { mv.NewTriangle(view.curStyle) })

	view.fillPreview = canvas.NewRectangle(defaultFill)
	view.fillPreview.SetMinSize(fyne.NewSize(24, 24))

	view.strokePreview = canvas.NewRectangle(defaultStroke)
	view.strokePreview.SetMinSize(fyne.NewSize(24, 24))

	btnFill := widget.NewButton("Fill", func() {
		d := dialog.NewColorPicker("Fill", "Select fill color", func(c color.Color) {
			var cc = c
			view.curStyle.Fill = &cc

			view.fillPreview.FillColor = c
			view.fillPreview.Refresh()

			mv.SetFillColor(c)
		}, win)
		d.Show()
	})

	btnStroke := widget.NewButton("Stroke", func() {
		d := dialog.NewColorPicker("Stroke", "Select stroke color", func(c color.Color) {
			var cc = c
			view.curStyle.Stroke = &cc

			view.strokePreview.FillColor = c
			view.strokePreview.Refresh()

			mv.SetBorderColor(c)
		}, win)
		d.Show()
	})

	view.btnImage = widget.NewButton("BackgroundImagePath", func() {
		d := dialog.NewFileOpen(func(rc fyne.URIReadCloser, err error) {
			if err != nil || rc == nil {
				return
			}
			path := rc.URI().Path()
			_ = rc.Close()

			mv.NewImage(path)
		}, win)

		d.SetFilter(storage.NewExtensionFileFilter([]string{".png", ".jpg", ".jpeg", ".bmp"}))
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

	view.obj = container.NewHBox(
		btnRect, btnEllipse, btnTriangle, view.btnImage,
		widget.NewSeparator(),
		container.NewHBox(btnFill, view.fillPreview),
		container.NewHBox(btnStroke, view.strokePreview),
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
	normalized := style

	if normalized.Fill == nil {
		c := color.Color(t.defaultFill)
		normalized.Fill = &c
	}
	if normalized.Stroke == nil {
		c := color.Color(t.defaultStroke)
		normalized.Stroke = &c
	}

	t.curStyle = normalized

	{
		if t.curStyle.Fill == nil {
			t.fillPreview.FillColor = t.defaultFill
		} else if rgba, ok := (*t.curStyle.Fill).(color.RGBA); ok {
			t.fillPreview.FillColor = rgba
		} else {
			r, g, b, a := (*t.curStyle.Fill).RGBA()
			t.fillPreview.FillColor = color.RGBA{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
		}
		t.fillPreview.Refresh()
	}

	{
		if t.curStyle.Stroke == nil {
			t.strokePreview.FillColor = t.defaultStroke
		} else if rgba, ok := (*t.curStyle.Stroke).(color.RGBA); ok {
			t.strokePreview.FillColor = rgba
		} else {
			r, g, b, a := (*t.curStyle.Stroke).RGBA()
			t.strokePreview.FillColor = color.RGBA{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
		}
		t.strokePreview.Refresh()
	}

	_ = selectedCount
}
