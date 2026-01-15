package view

import (
	"image"
	"image/color"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"

	"gioui.org/f32"
	"gioui.org/io/event"
	"gioui.org/io/key"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
)

const (
	HandleSize  = 10
	strokeWidth = 3
)

type ShapeView interface {
	Update()
	IsDeleted() bool
	IsSelected() bool
	Select(ctrl bool)

	GetPosition() geometry.Point
	GetBounds() geometry.Bounds

	StartDragging()
	StopDragging()
	Drag(delta geometry.Vector)
	// TODO: эту хуйню можно вынести

	StartResizing()
	StopResizing()
	Resize(delta geometry.Vector, scale geometry.Scale)

	Process(gtx layout.Context) layout.Dimensions
	ProcessHandles(gtx layout.Context) layout.Dimensions
}
type shapeView struct {
	mv     modelview.ShapeModelView
	canvas CanvasView

	handleTag int
}

func NewShapeView(
	mv modelview.ShapeModelView,
	canvas CanvasView,
) ShapeView {
	s := &shapeView{mv: mv, canvas: canvas}
	mv.AddObserver(s)

	return s
}

func (s *shapeView) Process(gtx layout.Context) layout.Dimensions {
	modelPos := s.GetPosition()
	modelSize := s.GetBounds()
	modelStyle := s.mv.GetStyle()

	pos := image.Point{X: modelPos.X, Y: modelPos.Y}
	size := image.Point{X: modelSize.Width, Y: modelSize.Height}
	fillColor := color.NRGBAModel.Convert(modelStyle.Fill).(color.NRGBA)
	strokeColor := color.NRGBAModel.Convert(modelStyle.Stroke).(color.NRGBA)

	rect := image.Rectangle{
		Min: pos,
		Max: pos.Add(size),
	}
	fillOp, strokeOp := s.getShapeOptions(gtx, rect)

	paint.FillShape(gtx.Ops, strokeColor, strokeOp)
	paint.FillShape(gtx.Ops, fillColor, fillOp)

	clipArea := fillOp.Push(gtx.Ops)
	event.Op(gtx.Ops, s)

	s.processClick(gtx)

	clipArea.Pop()
	return D{Size: size}
}

func (s *shapeView) getShapeOptions(
	gtx layout.Context,
	rect image.Rectangle,
) (fillOp clip.Op, strokeOp clip.Op) {
	var strokePath clip.PathSpec

	switch s.mv.GetShapeType() {
	case types.Rect:
		obj := clip.Rect(rect)
		fillOp = obj.Op()
		strokePath = obj.Path()

	case types.Ellipse:
		obj := clip.Ellipse(rect)
		fillOp = obj.Op(gtx.Ops)
		strokePath = obj.Path(gtx.Ops)

	case types.Triangle:
		var path clip.Path

		a, b, c := getTrianglePoints(rect)

		path.Begin(gtx.Ops)
		path.MoveTo(a)
		path.LineTo(b)
		path.LineTo(c)
		path.Close()

		strokePath = path.End()
		fillOp = clip.Outline{Path: strokePath}.Op()
	}
	strokeOp = clip.Stroke{Path: strokePath, Width: strokeWidth}.Op()
	return fillOp, strokeOp
}

func (s *shapeView) processClick(gtx layout.Context) {
	for {
		ev, ok := gtx.Event(pointer.Filter{
			Target: s,
			Kinds:  pointer.Press | pointer.Release | pointer.Drag,
		})
		if !ok {
			break
		}
		pointerEv, ok := ev.(pointer.Event)
		if !ok {
			break
		}
		mouseEvent := mouseEvent{
			pos: geometry.Point{
				X: int(pointerEv.Position.X),
				Y: int(pointerEv.Position.Y),
			},
			ctrl: pointerEv.Modifiers.Contain(key.ModCtrl),
		}
		switch pointerEv.Kind {
		case pointer.Drag:
			s.canvas.CurrentState().OnMouseMove(mouseEvent)
		case pointer.Press:
			s.canvas.CurrentState().OnShapeClick(mouseEvent, s)
		case pointer.Release:
			s.canvas.CurrentState().OnMouseUp(mouseEvent)
		default:
			panic("something goes wrong")
		}
	}
}

func pointInTriangle(p f32.Point, rect image.Rectangle) bool {
	a, b, c := getTrianglePoints(rect)
	asX := p.X - a.X
	asY := p.Y - a.Y

	sAB := (b.X-a.X)*asY-(b.Y-a.Y)*asX > 0

	if ((c.X-a.X)*asY-(c.Y-a.Y)*asX > 0) == sAB {
		return false
	}
	if ((c.X-b.X)*(p.Y-b.Y)-(c.Y-b.Y)*(p.X-b.X) > 0) != sAB {
		return false
	}
	return true
}

func getTrianglePoints(rect image.Rectangle) (f32.Point, f32.Point, f32.Point) {
	minX := float32(rect.Min.X)
	minY := float32(rect.Min.Y)

	w := float32(rect.Dx())
	h := float32(rect.Dy())

	top := f32.Pt(minX+w/2, minY)
	bottomRight := f32.Pt(minX+w, minY+h)
	bottomLeft := f32.Pt(minX, minY+h)

	return top, bottomRight, bottomLeft
}

func (s *shapeView) ProcessHandles(gtx layout.Context) layout.Dimensions {
	modelPos := s.GetPosition()
	modelSize := s.GetBounds()

	pos := image.Point{X: modelPos.X, Y: modelPos.Y}
	size := image.Point{X: modelSize.Width, Y: modelSize.Height}

	rect := image.Rectangle{Min: pos, Max: pos.Add(size)}

	selectionColor := color.NRGBA{R: 0, G: 120, B: 255, A: 255}

	borderPath := clip.Rect(rect).Path()
	borderStroke := clip.Stroke{Path: borderPath, Width: 2}.Op()
	paint.FillShape(gtx.Ops, selectionColor, borderStroke)

	halfHandle := HandleSize / 2
	getHandleRect := func(x, y int) image.Rectangle {
		p := image.Point{X: x - halfHandle, Y: y - halfHandle}
		return image.Rectangle{Min: p, Max: p.Add(image.Pt(HandleSize, HandleSize))}
	}

	tl := getHandleRect(rect.Min.X, rect.Min.Y)
	tr := getHandleRect(rect.Max.X, rect.Min.Y)
	bl := getHandleRect(rect.Min.X, rect.Max.Y)
	br := getHandleRect(rect.Max.X, rect.Max.Y)

	handles := []struct {
		rect image.Rectangle
		kind geometry.ResizeHandle
	}{
		{tl, geometry.HandleTopLeft},
		{tr, geometry.HandleTopRight},
		{bl, geometry.HandleBottomLeft},
		{br, geometry.HandleBottomRight},
	}

	for i := range handles {
		h := handles[i]
		handle := clip.Ellipse(h.rect)
		paint.FillShape(gtx.Ops, selectionColor, handle.Op(gtx.Ops))

		stack := handle.Push(gtx.Ops)

		event.Op(gtx.Ops, h)

		for {
			ev, ok := gtx.Event(pointer.Filter{
				Target: h,
				Kinds:  pointer.Press | pointer.Release | pointer.Drag,
			})
			if !ok {
				break
			}
			ptrEv, ok := ev.(pointer.Event)
			if !ok {
				break
			}
			mouseP := geometry.Point{
				X: int(ptrEv.Position.X),
				Y: int(ptrEv.Position.Y),
			}

			mouseEvent := mouseEvent{
				pos:  mouseP,
				ctrl: ptrEv.Modifiers.Contain(key.ModCtrl),
			}

			switch ptrEv.Kind {
			case pointer.Press:
				s.canvas.SetResizingState(mouseEvent, s, h.kind)
			case pointer.Drag:
				s.canvas.CurrentState().OnMouseMove(mouseEvent)
			case pointer.Release:
				s.canvas.CurrentState().OnMouseUp(mouseEvent)
			}
		}
		stack.Pop()
	}

	return layout.Dimensions{Size: size}
}

func hitHandle(p geometry.Point, handles []struct {
	rect image.Rectangle
	kind geometry.ResizeHandle
}) (geometry.ResizeHandle, bool) {
	imgP := image.Point{X: p.X, Y: p.Y}
	for _, h := range handles {
		if imgP.In(h.rect) {
			return h.kind, true
		}
	}
	return 0, false
}

func (s *shapeView) Update() {
	s.canvas.Invalidate()
}

func (s *shapeView) IsDeleted() bool {
	return s.mv.IsDeleted()
}

func (s *shapeView) IsSelected() bool {
	return s.mv.IsSelected()
}

func (s *shapeView) GetPosition() geometry.Point {
	return s.mv.GetPosition()
}

func (s *shapeView) GetBounds() geometry.Bounds {
	return s.mv.GetBounds()
}

func (s *shapeView) Select(ctrl bool) {
	s.mv.Select(ctrl)
}

func (s *shapeView) StartDragging() { s.mv.StartDragging() }
func (s *shapeView) StopDragging()  { s.mv.StopDragging() }
func (s *shapeView) Drag(delta geometry.Vector) {
	s.mv.Drag(delta)
}

func (s *shapeView) StartResizing() { s.mv.StartResizing() }
func (s *shapeView) StopResizing()  { s.mv.StopResizing() }
func (s *shapeView) Resize(delta geometry.Vector, scale geometry.Scale) {
	s.mv.Resize(delta, scale)
}

//		if s.mv.GetStyle().BackgroundImagePath == nil {
//			drawRect(img, pos, b, fill, stroke)
//		} else {
//			drawRectImage(
//				img,
//				pos,
//				b,
//				file.Open(*s.mv.GetStyle().BackgroundImagePath),
//			)
//		}

//func (s *shapeView) DrawSelection(img *image.RGBA) {
//	stroke := color.RGBA{0, 120, 255, 255}
//	handle := 8.0
//	pos := s.mv.GetPosition()
//	b := s.mv.GetBounds()
//	drawRect(img, pos, b, color.RGBA{0, 0, 0, 0}, stroke)
//
//	drawRect(img, pos, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
//	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
//	drawRect(img, geometry.Point{X: pos.X, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
//	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
//}
