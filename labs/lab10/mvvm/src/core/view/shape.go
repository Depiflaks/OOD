package view

import (
	"image"
	"image/color"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"

	"gioui.org/f32"
	"gioui.org/gesture"
	"gioui.org/io/event"
	"gioui.org/io/key"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op"
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

	drag gesture.Drag
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

	rect := image.Rectangle{Max: size}
	offset := op.Offset(pos).Push(gtx.Ops)
	fillOp, strokeOp := s.getShapeOptions(gtx, rect)

	paint.FillShape(gtx.Ops, strokeColor, strokeOp)
	paint.FillShape(gtx.Ops, fillColor, fillOp)

	clipArea := fillOp.Push(gtx.Ops)
	event.Op(gtx.Ops, s)
	//s.drag.Add(gtx.Ops)

	s.processClick(gtx, pos)

	clipArea.Pop()
	offset.Pop()
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

func (s *shapeView) processClick(gtx layout.Context, offset image.Point) {
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
				X: int(pointerEv.Position.X) + offset.X,
				Y: int(pointerEv.Position.Y) + offset.Y,
			},
			ctrl: pointerEv.Modifiers.Contain(key.ModCtrl),
		}
		//fmt.Println("mouseEvent:", mouseEvent)
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
	w := float32(rect.Dx())
	h := float32(rect.Dy())

	return f32.Pt(w/2, 0), f32.Pt(w, h), f32.Pt(0, h)
}

func (s *shapeView) ProcessHandles(gtx layout.Context) layout.Dimensions {
	//TODO implement me
	panic("implement me")
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
