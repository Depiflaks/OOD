package main

import (
	"fmt"
	"image"
	"image/color"
	"log"
	"os"

	"gioui.org/app"
	"gioui.org/f32"
	"gioui.org/font/gofont"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/text"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/colorpicker"
)

func main() {
	go func() {
		w := new(app.Window)
		w.Option(app.Title("Gio Shapes"), app.Size(500, 700))
		if err := loop(w); err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}()
	app.Main()
}

type (
	C = layout.Context
	D = layout.Dimensions
)

func loop(w *app.Window) error {
	th := material.NewTheme()
	th.Shaper = text.NewShaper(text.WithCollection(gofont.Collection()))

	var btn1, btn2 widget.Clickable
	var rectBtn, ovalBtn, triBtn widget.Clickable

	picker := colorpicker.State{}
	picker.SetColor(color.NRGBA{R: 0, G: 150, B: 255, A: 255})

	var ops op.Ops

	for {
		switch e := w.Event().(type) {
		case app.DestroyEvent:
			return e.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			if btn1.Clicked(gtx) {
				fmt.Println("Button 1 Clicked")
			}
			if btn2.Clicked(gtx) {
				fmt.Println("Button 2 Clicked")
			}
			if rectBtn.Clicked(gtx) {
				fmt.Println("Rectangle Clicked")
			}
			if ovalBtn.Clicked(gtx) {
				fmt.Println("Ellipse Clicked")
			}
			if triBtn.Clicked(gtx) {
				fmt.Println("Triangle Clicked")
			}
			if picker.Update(gtx) {
				c := picker.Color()
				fmt.Printf("Color selected: R:%d G:%d B:%d\n", c.R, c.G, c.B)
			}

			currentColor := picker.Color()

			layout.Flex{
				Axis:    layout.Vertical,
				Spacing: layout.SpaceEvenly,
			}.Layout(gtx,
				layout.Rigid(func(gtx C) D {
					return layout.UniformInset(10).Layout(gtx, func(gtx C) D {
						return material.Button(th, &btn1, "Print Hello").Layout(gtx)
					})
				}),
				layout.Rigid(func(gtx C) D {
					return layout.UniformInset(10).Layout(gtx, func(gtx C) D {
						return material.Button(th, &btn2, "Print World").Layout(gtx)
					})
				}),
				layout.Rigid(func(gtx C) D {
					return layout.UniformInset(10).Layout(gtx, func(gtx C) D {
						return colorpicker.PickerStyle{
							Theme:         th,
							State:         &picker,
							Label:         "Select Color",
							MonospaceFace: "Go Mono",
						}.Layout(gtx)
					})
				}),
				layout.Rigid(func(gtx C) D {
					return layout.Flex{
						Axis:    layout.Horizontal,
						Spacing: layout.SpaceEvenly,
					}.Layout(gtx,
						layout.Rigid(func(gtx C) D {
							return rectBtn.Layout(gtx, func(gtx C) D {
								size := image.Pt(80, 80)
								rect := clip.Rect{Max: size}
								paint.FillShape(gtx.Ops, currentColor, rect.Op())
								return D{Size: size}
							})
						}),
						layout.Rigid(func(gtx C) D {
							return ovalBtn.Layout(gtx, func(gtx C) D {
								size := image.Pt(80, 80)
								ellipse := clip.Ellipse{Max: size}
								paint.FillShape(gtx.Ops, currentColor, ellipse.Op(gtx.Ops))
								return D{Size: size}
							})
						}),
						layout.Rigid(func(gtx C) D {
							return triBtn.Layout(gtx, func(gtx C) D {
								size := image.Pt(80, 80)
								var path clip.Path
								path.Begin(gtx.Ops)
								path.MoveTo(f32.Point{X: float32(size.X / 2)})
								path.LineTo(f32.Point{X: float32(size.X), Y: float32(size.Y)})
								path.LineTo(f32.Point{Y: float32(size.Y)})
								path.Close()
								paint.FillShape(gtx.Ops, currentColor, clip.Outline{Path: path.End()}.Op())
								return D{Size: size}
							})
						}),
					)
				}),
			)

			e.Frame(gtx.Ops)
		}
	}
}
