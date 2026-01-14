package view

import (
	"fmt"
	"image"
	"image/color"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"

	"gioui.org/f32"
	"gioui.org/io/event"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
)

const HandleSize = 10.0

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

	Hit(p geometry.Point) bool
	HitHandle(p geometry.Point) (geometry.ResizeHandle, bool)

	Process(gtx layout.Context) layout.Dimensions
	ProcessHandles(gtx layout.Context) layout.Dimensions
}
type shapeView struct {
	mv     modelview.ShapeModelView
	canvas CanvasView
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
	pos := image.Point{X: modelPos.X, Y: modelPos.Y}
	size := image.Point{X: modelSize.Width, Y: modelSize.Height}

	modelStyle := s.mv.GetStyle()

	fillColor := color.NRGBAModel.Convert(modelStyle.Fill).(color.NRGBA)
	strokeColor := color.NRGBAModel.Convert(modelStyle.Stroke).(color.NRGBA)
	strokeWidth := float32(2.0)

	rect := image.Rectangle{Max: size}
	offset := op.Offset(pos).Push(gtx.Ops)

	var strokePath clip.PathSpec
	switch s.mv.GetShapeType() {

	// TODO: оптимизировать
	//TODO: общий тип - OP
	case types.Rect:
		cl := clip.Rect(rect)
		paint.FillShape(gtx.Ops, fillColor, cl.Op())
		strokePath = cl.Path()

	case types.Ellipse:
		cl := clip.Ellipse(rect)
		paint.FillShape(gtx.Ops, fillColor, cl.Op(gtx.Ops))
		strokePath = cl.Path(gtx.Ops)

	case types.Triangle:
		var path clip.Path
		path.Begin(gtx.Ops)

		w := float32(size.X)
		h := float32(size.Y)

		path.MoveTo(f32.Pt(w/2, 0))
		path.LineTo(f32.Pt(w, h))
		path.LineTo(f32.Pt(0, h))
		path.Close()

		strokePath = path.End()
		cl := clip.Outline{Path: strokePath}.Op()

		paint.FillShape(gtx.Ops, fillColor, cl.Op())
	}
	strokeShape := clip.Stroke{Path: strokePath, Width: strokeWidth}.Op()
	paint.FillShape(gtx.Ops, strokeColor, strokeShape)

	// TODO: заменить на нормальную область
	shapeRect := clip.Rect{Max: size}
	clipArea := shapeRect.Push(gtx.Ops)
	event.Op(gtx.Ops, s)

	for {
		ev, ok := gtx.Event(pointer.Filter{
			Target: s,
			Kinds:  pointer.Press | pointer.Release,
		})
		if !ok {
			break
		}
		switch ev.(pointer.Event).Kind {
		case pointer.Press:
			fmt.Println("shape pressed")
		case pointer.Release:
			fmt.Println("shape released")
		}
	}

	clipArea.Pop()
	offset.Pop()
	return D{Size: size}
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

func (s *shapeView) Hit(p geometry.Point) bool {
	//pos := s.mv.GetPosition()
	//b := s.mv.GetBounds()
	//t := s.mv.GetShapeType()
	return false
	//return geometry.HitShape(p, pos, b, t)
}

func (s *shapeView) HitHandle(p geometry.Point) (geometry.ResizeHandle, bool) {
	//pos := s.mv.GetPosition()
	//b := s.mv.GetBounds()

	//return geometry.HitHandle(p, pos, b, HandleSize)
	return geometry.HandleTopLeft, false
}

//func (s *shapeView) Draw(img *image.RGBA) {
//	if s.mv.IsDeleted() {
//		return
//	}
//
//	pos := s.mv.GetPosition()
//	b := s.mv.GetBounds()
//
//	fill := rgbaFromPtr(s.mv.GetStyle().Fill)
//	stroke := rgbaFromPtr(s.mv.GetStyle().Stroke)
//
//	switch s.mv.GetShapeType() {
//	case types.Rect:
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
//	case types.Ellipse:
//		drawEllipse(img, pos, b, fill, stroke)
//	case types.Triangle:
//		drawTriangle(img, pos, b, fill, stroke)
//	}
//}
//
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
