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

const verticalSpace = 10

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

	return layout.UniformInset(unit.Dp(10)).Layout(gtx, func(gtx C) D {
		return layout.Flex{Axis: layout.Horizontal, Alignment: layout.Middle}.Layout(gtx,
			layout.Rigid(func(gtx C) D {
				return t.drawShapeButtons(gtx)
			}),
			layout.Rigid(layout.Spacer{Width: unit.Dp(40)}.Layout),
			layout.Rigid(func(gtx C) D {
				gtx.Constraints = layout.Exact(buttonSize)
				return material.Button(t.buttonTheme, &t.btnImage, "Image").Layout(gtx)
			}),
			layout.Rigid(layout.Spacer{Width: unit.Dp(10)}.Layout),
			layout.Rigid(func(gtx C) D {
				return t.drawColorPickers(gtx)
			}),
			layout.Rigid(layout.Spacer{Width: unit.Dp(40)}.Layout),
			layout.Rigid(func(gtx C) D {
				return t.drawFileOperationButtons(gtx)
			}),
		)
	})
}

func (t *toolbarView) drawFileOperationButtons(gtx layout.Context) layout.Dimensions {
	return layout.Flex{
		Axis:      layout.Vertical,
		Spacing:   layout.SpaceBetween,
		Alignment: layout.Middle,
	}.Layout(gtx,
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnSave, "Save").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Height: unit.Dp(verticalSpace)}.Layout),
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnSaveAs, "SaveAs").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Height: unit.Dp(verticalSpace)}.Layout),
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnOpen, "Open").Layout(gtx)
		}),
	)
}

func (t *toolbarView) drawShapeButtons(gtx layout.Context) layout.Dimensions {
	return layout.Flex{
		Axis:      layout.Horizontal,
		Spacing:   layout.SpaceBetween,
		Alignment: layout.Middle,
	}.Layout(gtx,
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnRect, "Rect").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Width: unit.Dp(verticalSpace)}.Layout),
		layout.Rigid(func(gtx C) D {
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, &t.btnTri, "Tri").Layout(gtx)
		}),
		layout.Rigid(layout.Spacer{Width: unit.Dp(verticalSpace)}.Layout),
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

		layout.Rigid(layout.Spacer{Height: unit.Dp(verticalSpace)}.Layout),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return t.drawColorRow(gtx, "Stroke", t.stroke, &t.btnStroke)
		}),

		layout.Rigid(layout.Spacer{Height: unit.Dp(verticalSpace)}.Layout),

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
			gtx.Constraints = layout.Exact(buttonSize)
			return material.Button(t.buttonTheme, btn, label).Layout(gtx)
		}),

		layout.Rigid(layout.Spacer{Width: unit.Dp(8)}.Layout),

		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			square := image.Rectangle{Max: buttonSize}

			rr := clip.UniformRRect(square, 3)
			paint.FillShape(gtx.Ops, col, rr.Op(gtx.Ops))

			stroke := clip.Stroke{Path: rr.Path(gtx.Ops), Width: 1}
			paint.FillShape(gtx.Ops, color.NRGBA{A: 50}, stroke.Op())

			return layout.Dimensions{Size: rr.Rect.Size()}
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
				t.mv.SetFillColor(c)
			}
		}()
	}
	if t.btnStroke.Clicked(gtx) {
		go func() {
			c, err := openColorPickerDialog()
			if err == nil {
				t.mv.SetBorderColor(c)
			}
		}()
	}
	if t.btnBackground.Clicked(gtx) {
		// TODO: отдельная обработка background
	}

	if t.btnRect.Clicked(gtx) {
		t.mv.NewRectangle(draw.Style{
			Fill:   t.fill,
			Stroke: t.stroke,
		})
	}
	if t.btnOval.Clicked(gtx) {
		t.mv.NewEllipse(draw.Style{
			Fill:   t.fill,
			Stroke: t.stroke,
		})
	}
	if t.btnTri.Clicked(gtx) {
		t.mv.NewTriangle(draw.Style{
			Fill:   t.fill,
			Stroke: t.stroke,
		})
	}

	if t.btnImage.Clicked(gtx) {
		fmt.Println("Action: Load Image")
		go func() {
			path, err := openFileDialog("Images", []string{".png", ".jpg", ".jpeg", ".bmp"})
			if err == nil {
				t.mv.NewImage(path)
			}
		}()
	}

	if t.btnOpen.Clicked(gtx) {
		go t.fileActions.Open()
	}

	if t.btnSave.Clicked(gtx) {
		t.fileActions.Save()
	}

	if t.btnSaveAs.Clicked(gtx) {
		go t.fileActions.SaveAs()
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
