package view

import (
	"fmt"
	"image"
	"image/color"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types/draw"

	"gioui.org/app"
	"gioui.org/font/gofont"
	"gioui.org/layout"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/text"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/colorpicker"
	"github.com/ncruces/zenity"
)

type ToolbarView interface {
	Process(gtx layout.Context) layout.Dimensions
}

var (
	defaultFill   = color.NRGBA{R: 255, G: 3, B: 34, A: 255}
	defaultStroke = color.NRGBA{R: 0, G: 0, B: 0, A: 255}
	white         = color.NRGBA{R: 0xff, G: 0xff, B: 0xff, A: 0xff}

	buttonColor = color.NRGBA{R: 33, G: 150, B: 243, A: 255}
	buttonSize  = image.Point{X: 75, Y: 40}
)

type muxOption string

const (
	fillOption       muxOption = "fill"
	strokeOption     muxOption = "stroke"
	backgroundOption muxOption = "background"
)

type toolbarView struct {
	window      *app.Window
	mv          modelview.ToolbarModelView
	fileActions FileActions
	locker      Locker

	background color.NRGBA
	stroke     color.NRGBA
	fill       color.NRGBA

	buttonTheme   *material.Theme
	currentOption muxOption

	colorPicker                        *colorpicker.State
	muxState                           colorpicker.MuxState
	btnRect, btnTri, btnOval, btnImage widget.Clickable
	btnFill, btnStroke, btnBackground  widget.Clickable
	btnSave, btnSaveAs, btnOpen        widget.Clickable
}

func NewToolbarView(
	window *app.Window,
	mv modelview.ToolbarModelView,
	fileActions FileActions,
	locker Locker,
) ToolbarView {
	view := &toolbarView{
		window:        window,
		mv:            mv,
		background:    white,
		fill:          defaultFill,
		stroke:        defaultStroke,
		currentOption: backgroundOption,
		fileActions:   fileActions,
		locker:        locker,
	}

	view.buttonTheme = material.NewTheme()
	view.buttonTheme.ContrastBg = buttonColor
	view.buttonTheme.Shaper = text.NewShaper(text.WithCollection(gofont.Collection()))

	view.colorPicker = &colorpicker.State{}
	view.updatePreviewColor()
	view.muxState = colorpicker.NewMuxState(
		[]colorpicker.MuxOption{
			{
				Label: string(fillOption),
				Value: &view.fill,
			},
			{
				Label: string(strokeOption),
				Value: &view.stroke,
			},
			{
				Label: string(backgroundOption),
				Value: &view.background,
			},
		}...)

	mv.AddObserver(view)
	return view
}

func (t *toolbarView) Process(gtx layout.Context) layout.Dimensions {
	t.processButtonsClick(gtx)

	rect := image.Rectangle{Max: gtx.Constraints.Max}
	areaStack := clip.Rect(rect).Push(gtx.Ops)
	paint.Fill(gtx.Ops, color.NRGBA{R: 255, G: 250, B: 240, A: 255})
	areaStack.Pop()

	layout.UniformInset(unit.Dp(10)).Layout(gtx, func(gtx C) D {
		return layout.Flex{Axis: layout.Horizontal}.Layout(gtx,
			layout.Rigid(func(gtx C) D {
				return t.drawShapeButtons(gtx)
			}),
			layout.Rigid(layout.Spacer{Width: unit.Dp(20)}.Layout),
			layout.Rigid(func(gtx C) D {
				return t.drawColorPickers(gtx)
			}),

			//layout.Rigid(func(gtx C) D {
			//	return layout.UniformInset(unit.Dp(5)).Layout(gtx, func(gtx C) D {
			//		return layout.Flex{Axis: layout.Horizontal, Spacing: layout.SpaceEvenly}.Layout(gtx,
			//			layout.Rigid(func(gtx C) D { return material.Button(th, &btnUndo, "Undo").Layout(gtx) }),
			//			layout.Rigid(func(gtx C) D { return material.Button(th, &btnRedo, "Redo").Layout(gtx) }),
			//			layout.Rigid(func(gtx C) D { return layout.Spacer{Width: unit.Dp(20)}.Layout(gtx) }),
			//			layout.Rigid(func(gtx C) D { return material.Button(th, &btnSave, "Save").Layout(gtx) }),
			//			layout.Rigid(func(gtx C) D { return material.Button(th, &btnSaveAs, "Save As").Layout(gtx) }),
			//			layout.Rigid(func(gtx C) D { return material.Button(th, &btnOpen, "Open").Layout(gtx) }),
			//		)
			//	})
			//}),
			//
			//layout.Flexed(1, func(gtx C) D {
			//	size := gtx.Constraints.Max
			//
			//	defer clip.Rect{Max: size}.Push(gtx.Ops).Pop()
			//
			//	paint.Fill(gtx.Ops, color.NRGBA{R: 40, G: 40, B: 45, A: 255})
			//
			//	center := image.Pt(size.X, size.Y)
			//	paint.FillShape(gtx.Ops,
			//		color.NRGBA{R: 255, G: 255, B: 255, A: 50},
			//		clip.Ellipse{Min: center.Sub(image.Pt(100, 100)), Max: center.Add(image.Pt(100, 100))}.Op(gtx.Ops),
			//	)
			//
			//	return D{Size: size}
			//}),
		)
	})
	//btnFill := widget.NewButton("Fill", func() {
	//	d := dialog.NewColorPicker("Fill", "Select fill color", func(c color.Color) {
	//		var cc = c
	//		view.curStyle.Fill = &cc
	//
	//		view.fillPreview.FillColor = c
	//		view.fillPreview.Refresh()
	//
	//		mv.SetFillColor(c)
	//	}, window)
	//	d.Show()
	//})
	//
	//btnStroke := widget.NewButton("Stroke", func() {
	//	d := dialog.NewColorPicker("Stroke", "Select stroke color", func(c color.Color) {
	//		var cc = c
	//		view.curStyle.Stroke = &cc
	//
	//		view.strokePreview.FillColor = c
	//		view.strokePreview.Refresh()
	//
	//		mv.SetBorderColor(c)
	//	}, window)
	//	d.Show()
	//})
	//
	return layout.Dimensions{}
}

func (t *toolbarView) drawShapeButtons(gtx layout.Context) layout.Dimensions {
	return layout.Flex{
		Axis:      layout.Vertical,
		Spacing:   layout.SpaceBetween,
		Alignment: layout.Middle,
	}.Layout(gtx,
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnRect, "Rect").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Height: unit.Dp(6)}.Layout),
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnTri, "Tri").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Height: unit.Dp(6)}.Layout),
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnOval, "Oval").Layout(gtx)
		}),
	)
}

func (t *toolbarView) drawColorPickers(gtx layout.Context) layout.Dimensions {
	return layout.Flex{
		Axis:      layout.Vertical,
		Spacing:   layout.SpaceBetween,
		Alignment: layout.Middle,
	}.Layout(gtx,
		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return t.drawColorRow(gtx, "Fill", t.fill, &t.btnFill)
		}),

		layout.Rigid(layout.Spacer{Height: unit.Dp(6)}.Layout),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return t.drawColorRow(gtx, "Stroke", t.stroke, &t.btnStroke)
		}),

		layout.Rigid(layout.Spacer{Height: unit.Dp(6)}.Layout),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return t.drawColorRow(gtx, "Background", t.background, &t.btnBackground)
		}),
	)
}
func (t *toolbarView) drawColorRow(
	gtx layout.Context,
	label string,
	col color.NRGBA,
	btn *widget.Clickable,
) layout.Dimensions {
	return layout.Flex{
		Axis: layout.Horizontal,
	}.Layout(gtx,
		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			square := image.Rectangle{Min: buttonSize}

			rr := clip.UniformRRect(square, 3)
			paint.FillShape(gtx.Ops, col, rr.Op(gtx.Ops))

			stroke := clip.Stroke{Path: rr.Path(gtx.Ops), Width: 1}
			paint.FillShape(gtx.Ops, color.NRGBA{A: 50}, stroke.Op())

			return layout.Dimensions{Size: rr.Rect.Size()}
		}),

		layout.Rigid(layout.Spacer{Width: unit.Dp(8)}.Layout),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, btn, label).Layout(gtx)
		}),
	)
}

func (t *toolbarView) updatePreviewColor() {
	switch t.currentOption {
	case fillOption:
		t.colorPicker.SetColor(t.fill)
	case backgroundOption:
		t.colorPicker.SetColor(t.background)
	case strokeOption:
		t.colorPicker.SetColor(t.stroke)
	}
}

func (t *toolbarView) processButtonsClick(gtx layout.Context) {
	if t.btnFill.Clicked(gtx) {
		go func() {
			c, err := openColorPickerDialog()
			if err == nil {
				t.mv.SetBorderColor(c)
			}
		}()
	}
	if t.btnStroke.Clicked(gtx) {
		go func() {
			c, err := openColorPickerDialog()
			if err == nil {
				t.mv.SetFillColor(c)
			}
		}()
	}
	if t.btnBackground.Clicked(gtx) {
		// TODO: отдельная обработка background
	}

	if t.btnRect.Clicked(gtx) {
		t.mv.NewRectangle(draw.Style{
			Fill:   t.background,
			Stroke: t.stroke,
		})
	}
	if t.btnOval.Clicked(gtx) {
		t.mv.NewEllipse(draw.Style{
			Fill:   t.background,
			Stroke: t.stroke,
		})
	}
	if t.btnTri.Clicked(gtx) {
		t.mv.NewTriangle(draw.Style{
			Fill:   t.background,
			Stroke: t.stroke,
		})
	}

	if t.btnImage.Clicked(gtx) {
		fmt.Println("Action: Load Image")
		go func() {
			path, err := openFileDialog([]string{"png", "jpg", "jpeg", "bmp"})
			if err == nil {
				t.mv.NewImage(path)
			}
		}()
	}

	if t.btnOpen.Clicked(gtx) {
		fmt.Println("File: Open")
		if t.fileActions.Open != nil {
			go func() {
				path, err := zenity.SelectFile(zenity.Directory())
				if err == nil {
					t.fileActions.Open(path)
					// TODO: возможно надо обновлять экран
					//w.Invalidate()
				}
			}()
		}
	}

	if t.btnSave.Clicked(gtx) {
		fmt.Println("File: Save")
		t.fileActions.Save()
	}

	if t.btnSaveAs.Clicked(gtx) {
		fmt.Println("File: Save As")
		if t.fileActions.SaveAs != nil {
			go func() {
				path, err := openFolderDialog()
				if err == nil {
					t.fileActions.SaveAs(path)
					// TODO: возможно надо обновлять экран
					//w.Invalidate()
				}
			}()
		}
	}
}

func (t *toolbarView) OnSelectionChange(style draw.Style, selectedCount int) {
	normalized := style

	if normalized.Fill == nil {
		c := color.Color(defaultFill)
		normalized.Fill = c
	}
	if normalized.Stroke == nil {
		c := color.Color(defaultStroke)
		normalized.Stroke = c
	}

	{
		//if t.curStyle.Fill == nil {
		//	t.fillPreview.FillColor = t.defaultFill
		//} else if rgba, ok := (*t.curStyle.Fill).(color.RGBA); ok {
		//	t.fillPreview.FillColor = rgba
		//} else {
		//	r, g, b, a := (*t.curStyle.Fill).RGBA()
		//	t.fillPreview.FillColor = color.RGBA{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
		//}
		//t.fillPreview.Refresh()
	}

	{
		//if t.curStyle.Stroke == nil {
		//	t.strokePreview.FillColor = t.defaultStroke
		//} else if rgba, ok := (*t.curStyle.Stroke).(color.RGBA); ok {
		//	t.strokePreview.FillColor = rgba
		//} else {
		//	r, g, b, a := (*t.curStyle.Stroke).RGBA()
		//	t.strokePreview.FillColor = color.RGBA{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
		//}
		//t.strokePreview.Refresh()
	}

	_ = selectedCount
}

func openColorPickerDialog() (color.Color, error) {
	return zenity.SelectColor()
}

func openFileDialog(extensions []string) (string, error) {
	return zenity.SelectFile(
		zenity.FileFilters{
			{Name: "Images", Patterns: extensions},
		})
}

func openFolderDialog() (string, error) {
	return zenity.SelectFile(zenity.Directory())
}
