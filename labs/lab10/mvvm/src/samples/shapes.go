package samples

import (
	"image"
	"image/color"

	"gioui.org/app"
	"gioui.org/f32"
	"gioui.org/io/event"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/unit"
)

type State struct {
	IsPressed bool
}

func TriangleLoop(w *app.Window) error {
	var ops op.Ops

	state := &State{}

	for {
		switch e := w.Event().(type) {
		case app.DestroyEvent:
			return e.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			paint.Fill(gtx.Ops, color.NRGBA{R: 255, G: 255, B: 255, A: 255})

			layout.Center.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
				return drawInteractiveTriangle(gtx, state)
			})

			e.Frame(gtx.Ops)
		}
	}
}

func drawInteractiveTriangle(gtx layout.Context, s *State) layout.Dimensions {
	size := gtx.Dp(unit.Dp(200))
	dims := layout.Dimensions{Size: image.Pt(size, size)}

	w, h := float32(size), float32(size)

	var path clip.Path
	path.Begin(gtx.Ops)
	path.MoveTo(f32.Pt(w/2, 0))
	path.LineTo(f32.Pt(w, h))
	path.LineTo(f32.Pt(0, h))
	path.Close()

	pathSpec := path.End()
	triangleOp := clip.Outline{Path: pathSpec}.Op()
	stack := triangleOp.Push(gtx.Ops)

	for {
		ev, ok := gtx.Event(pointer.Filter{
			Target: s,
			Kinds:  pointer.Press | pointer.Release,
		})
		if !ok {
			break
		}
		if x, ok := ev.(pointer.Event); ok {
			switch x.Kind {
			case pointer.Press:
				s.IsPressed = true
			case pointer.Release:
				s.IsPressed = false
			}
		}
	}

	event.Op(gtx.Ops, s)

	col := color.NRGBA{R: 0, G: 0, B: 255, A: 255}
	if s.IsPressed {
		col = color.NRGBA{R: 255, G: 0, B: 0, A: 255}
	}

	paint.FillShape(gtx.Ops, col, triangleOp)

	stack.Pop()

	return dims
}
