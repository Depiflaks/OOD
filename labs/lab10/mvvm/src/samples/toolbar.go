package samples

import (
	"fmt"
	"image"
	"image/color"

	"gioui.org/app"
	"gioui.org/font/gofont"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/text"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/colorpicker"
)

type (
	C = layout.Context
	D = layout.Dimensions
)

func ToolbarLoop(w *app.Window) error {
	th := material.NewTheme()
	th.Shaper = text.NewShaper(text.WithCollection(gofont.Collection()))

	var (
		btnRect, btnTri, btnOval    widget.Clickable
		btnSave, btnSaveAs, btnOpen widget.Clickable
	)

	picker := colorpicker.State{}
	picker.SetColor(color.NRGBA{R: 255, G: 0, B: 0, A: 255})

	var ops op.Ops

	for {
		switch e := w.Event().(type) {
		case app.DestroyEvent:
			return e.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			if btnRect.Clicked(gtx) {
				fmt.Println("Create: Rectangle")
			}
			if btnTri.Clicked(gtx) {
				fmt.Println("Create: Triangle")
			}
			if btnOval.Clicked(gtx) {
				fmt.Println("Create: Ellipse")
			}
			if btnSave.Clicked(gtx) {
				fmt.Println("File: Save")
			}
			if btnSaveAs.Clicked(gtx) {
				fmt.Println("File: Save As")
			}
			if btnOpen.Clicked(gtx) {
				fmt.Println("File: Open")
			}

			if picker.Update(gtx) {
				c := picker.Color()
				fmt.Printf("Color: %d %d %d\n", c.R, c.G, c.B)
			}

			layout.Flex{Axis: layout.Vertical}.Layout(gtx,

				layout.Rigid(func(gtx C) D {
					return layout.Flex{Axis: layout.Horizontal, Spacing: layout.SpaceBetween, Alignment: layout.Middle}.Layout(gtx,
						layout.Rigid(func(gtx C) D {
							return layout.Flex{}.Layout(gtx,
								layout.Rigid(func(gtx C) D { return material.Button(th, &btnRect, "Rect").Layout(gtx) }),
								layout.Rigid(func(gtx C) D {
									return layout.UniformInset(unit.Dp(2)).Layout(gtx, func(gtx C) D { return material.Button(th, &btnTri, "Tri").Layout(gtx) })
								}),
								layout.Rigid(func(gtx C) D { return material.Button(th, &btnOval, "Oval").Layout(gtx) }),
							)
						}),
						layout.Rigid(func(gtx C) D {
							return layout.UniformInset(unit.Dp(5)).Layout(gtx, func(gtx C) D {
								return colorpicker.PickerStyle{
									Theme: th,
									State: &picker,
									Label: "Color",
								}.Layout(gtx)
							})
						}),
					)
				}),

				layout.Rigid(func(gtx C) D {
					return layout.UniformInset(unit.Dp(5)).Layout(gtx, func(gtx C) D {
						return layout.Flex{Axis: layout.Horizontal, Spacing: layout.SpaceEvenly}.Layout(gtx,
							layout.Rigid(func(gtx C) D { return material.Button(th, &btnUndo, "Undo").Layout(gtx) }),
							layout.Rigid(func(gtx C) D { return material.Button(th, &btnRedo, "Redo").Layout(gtx) }),
							layout.Rigid(func(gtx C) D { return layout.Spacer{Width: unit.Dp(20)}.Layout(gtx) }),
							layout.Rigid(func(gtx C) D { return material.Button(th, &btnSave, "Save").Layout(gtx) }),
							layout.Rigid(func(gtx C) D { return material.Button(th, &btnSaveAs, "Save As").Layout(gtx) }),
							layout.Rigid(func(gtx C) D { return material.Button(th, &btnOpen, "Open").Layout(gtx) }),
						)
					})
				}),

				layout.Flexed(1, func(gtx C) D {
					size := gtx.Constraints.Max

					defer clip.Rect{Max: size}.Push(gtx.Ops).Pop()

					paint.Fill(gtx.Ops, color.NRGBA{R: 40, G: 40, B: 45, A: 255})

					center := image.Pt(size.X, size.Y)
					paint.FillShape(gtx.Ops,
						color.NRGBA{R: 255, G: 255, B: 255, A: 50},
						clip.Ellipse{Min: center.Sub(image.Pt(100, 100)), Max: center.Add(image.Pt(100, 100))}.Op(gtx.Ops),
					)

					return D{Size: size}
				}),
			)

			e.Frame(gtx.Ops)
		}
	}
}
