package view

import (
	"image"
	"image/color"
	"os"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"

	"gioui.org/f32"
	"gioui.org/io/event"
	"gioui.org/io/key"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/widget"
)

const (
	HandleSize  = 14
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
	mv        modelview.ShapeModelView
	canvas    CanvasView
	imgWidget widget.Image

	handles map[geometry.ResizeHandle]image.Rectangle
}

func NewShapeView(
	mv modelview.ShapeModelView,
	canvas CanvasView,
) ShapeView {
	s := &shapeView{mv: mv, canvas: canvas}

	s.handles = make(map[geometry.ResizeHandle]image.Rectangle)

	mv.AddObserver(s)

	return s
}

func (s *shapeView) updateHandles(rect image.Rectangle) {
	s.handles[geometry.HandleTopLeft] = getHandleRect(rect.Min.X, rect.Min.Y)
	s.handles[geometry.HandleTopRight] = getHandleRect(rect.Max.X, rect.Min.Y)
	s.handles[geometry.HandleBottomLeft] = getHandleRect(rect.Min.X, rect.Max.Y)
	s.handles[geometry.HandleBottomRight] = getHandleRect(rect.Max.X, rect.Max.Y)
}

func getHandleRect(x int, y int) image.Rectangle {
	halfHandle := HandleSize / 2
	p := image.Point{X: x - halfHandle, Y: y - halfHandle}
	return image.Rectangle{Min: p, Max: p.Add(image.Pt(HandleSize, HandleSize))}
}

func (s *shapeView) Process(gtx layout.Context) layout.Dimensions {
	modelPos := s.GetPosition()
	modelSize := s.GetBounds()

	pos := image.Point{X: modelPos.X, Y: modelPos.Y}
	size := image.Point{X: modelSize.Width, Y: modelSize.Height}

	rect := image.Rectangle{
		Min: pos,
		Max: pos.Add(size),
	}
	fillOp, strokeOp := s.getShapeOptions(gtx, rect)

	s.drawShape(gtx, strokeOp, fillOp, rect)

	clipArea := fillOp.Push(gtx.Ops)
	event.Op(gtx.Ops, s)

	s.processClick(gtx)

	clipArea.Pop()
	return D{Size: size}
}

func (s *shapeView) drawShape(
	gtx layout.Context,
	strokeOp clip.Op,
	fillOp clip.Op,
	rect image.Rectangle,
) {
	modelStyle := s.mv.GetStyle()
	fillColor := color.NRGBAModel.Convert(modelStyle.Fill).(color.NRGBA)
	strokeColor := color.NRGBAModel.Convert(modelStyle.Stroke).(color.NRGBA)
	paint.FillShape(gtx.Ops, fillColor, fillOp)
	s.drawImage(gtx, fillOp, rect)
	paint.FillShape(gtx.Ops, strokeColor, strokeOp)
}

func (s *shapeView) drawImage(
	gtx layout.Context,
	fillOp clip.Op,
	rect image.Rectangle,
) {
	pathPtr := s.mv.GetStyle().BackgroundImagePath
	if pathPtr == nil {
		return
	}
	f, err := os.Open(*pathPtr)
	if err != nil {
		return
	}
	img, _, err := image.Decode(f)
	f.Close()

	if err != nil {
		return
	}
	w := widget.Image{
		Src: paint.NewImageOp(img),
		Fit: widget.Fill,
	}

	clipStack := fillOp.Push(gtx.Ops)
	offStack := op.Offset(rect.Min).Push(gtx.Ops)

	gtx.Constraints = layout.Exact(rect.Size())

	w.Layout(gtx)

	offStack.Pop()
	clipStack.Pop()
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

	s.updateHandles(rect)

	for handleType, handleRect := range s.handles {
		handle := clip.Ellipse(handleRect)
		paint.FillShape(gtx.Ops, selectionColor, handle.Op(gtx.Ops))

		stack := handle.Push(gtx.Ops)

		event.Op(gtx.Ops, handleType)

		for {
			ev, ok := gtx.Event(pointer.Filter{
				Target: handleType,
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
			case pointer.Drag:
				s.canvas.CurrentState().OnMouseMove(mouseEvent)
			case pointer.Press:
				s.canvas.CurrentState().OnResizeActivated(mouseEvent, s, handleType)
			case pointer.Release:
				s.canvas.CurrentState().OnMouseUp(mouseEvent)
			}
		}
		stack.Pop()
	}

	return layout.Dimensions{Size: size}
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
