package main

import (
	"image"
	"image/color"
	"log"
	"os"

	"gioui.org/app"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/unit"
)

func main() {
	go func() {
		w := new(app.Window)
		w.Option(app.Title("Gio Click Demo"), app.Size(unit.Dp(400), unit.Dp(400)))

		if err := run(w); err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}()
	app.Main()
}

func run(w *app.Window) error {
	var ops op.Ops

	for {
		switch event := w.Event().(type) {
		case app.DestroyEvent:
			return event.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, event)
			paint.Fill(gtx.Ops, color.NRGBA{R: 30, G: 30, B: 35, A: 255})
			drawCircle(gtx)
			event.Frame(gtx.Ops)
		}
	}
}

func drawCircle(gtx layout.Context) {
	defer clip.Ellipse{
		Min: image.Pt(100, 100),
		Max: image.Pt(200, 200),
	}.Push(gtx.Ops).Pop()

	paint.ColorOp{Color: color.NRGBA{R: 255, G: 0, B: 0, A: 255}}.Add(gtx.Ops)
	paint.PaintOp{}.Add(gtx.Ops)
}
