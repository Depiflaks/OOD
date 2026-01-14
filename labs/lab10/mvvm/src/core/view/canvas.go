package view

import (
	"image/color"

	"gioui.org/app"
	"gioui.org/layout"
	"gioui.org/op/clip"
	"gioui.org/op/paint"

	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"
)

type mouseEvent struct {
	Pos  geometry.Point
	Ctrl bool
}

type State interface {
	OnShapeClick(e mouseEvent, shape ShapeView, ctrl bool)
	OnEmptyClick(e mouseEvent, ctrl bool)
	OnResizeActivated(e mouseEvent, shape ShapeView, marker geometry.ResizeHandle)
	OnMouseMove(e mouseEvent)
	OnMouseUp(e mouseEvent)
	OnMouseLeave()
}

type stateMachine interface {
	SetIdleState()
	SetDraggingState(e mouseEvent, active ShapeView)
	SetResizingState(e mouseEvent, active ShapeView, marker geometry.ResizeHandle)
}

type CanvasView interface {
	stateMachine

	Process(gtx layout.Context) layout.Dimensions

	ClearSelection()
	DeleteSelection()
	Invalidate()
}

type canvasView struct {
	window *app.Window
	mv     modelview.CanvasModelView

	shapes    map[types.ShapeId]ShapeView
	drawOrder []types.ShapeId

	current State
}

func (c *canvasView) OnBackgroundChanged() {
	c.Invalidate()
}

func (c *canvasView) SetIdleState() {
	c.current = NewIdleState(c)
}

func (c *canvasView) SetDraggingState(e mouseEvent, active ShapeView) {
	c.current = NewDraggingState(c, e, active)
}

func (c *canvasView) SetResizingState(e mouseEvent, active ShapeView, marker geometry.ResizeHandle) {
	c.current = NewResizingState(c, e, active, marker)
}

func NewCanvasView(
	window *app.Window,
	mv modelview.CanvasModelView,
) CanvasView {
	c := &canvasView{
		mv:     mv,
		window: window,
		shapes: make(map[types.ShapeId]ShapeView),
	}

	c.current = NewIdleState(c)

	mv.AddObserver(c)
	return c
}

func (c *canvasView) Process(gtx layout.Context) layout.Dimensions {
	c.fillBackground(gtx)

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() {
			continue
		}
		sv.Process(gtx)
	}

	//for _, id := range c.drawOrder {
	//	sv := c.shapes[id]
	//	if sv == nil || sv.IsDeleted() || !sv.IsSelected() {
	//		continue
	//	}
	//	sv.DrawSelection(c.img)
	//}

	return layout.Dimensions{Size: gtx.Constraints.Max}
}

func (c *canvasView) fillBackground(gtx layout.Context) {
	background := c.mv.GetBackgroundColor()
	clip.Rect{Max: gtx.Constraints.Max}.Push(gtx.Ops)
	col := color.NRGBAModel.Convert(background).(color.NRGBA)
	paint.Fill(gtx.Ops, col)
}

//func (c *canvasView) redraw() {
//	if c.img == nil {
//		return
//	}
//
//	draw.Draw(c.img, c.img.Bounds(), &image.Uniform{C: c.mv.GetBackgroundColor()}, image.Point{}, draw.Src)
//
//	for _, id := range c.drawOrder {
//		sv := c.shapes[id]
//		if sv == nil || sv.IsDeleted() {
//			continue
//		}
//		sv.Draw(c.img)
//	}
//
//	for _, id := range c.drawOrder {
//		sv := c.shapes[id]
//		if sv == nil || sv.IsDeleted() || !sv.IsSelected() {
//			continue
//		}
//		sv.DrawSelection(c.img)
//	}
//}

func (c *canvasView) OnShapesChanged(ids []types.ShapeId) {
	for _, id := range ids {
		shape := c.mv.GetShape(id)

		if shape == nil {
			if _, ok := c.shapes[id]; ok {
				delete(c.shapes, id)
			}
			continue
		}

		if _, ok := c.shapes[id]; ok {
			continue
		}
		sv := NewShapeView(shape, c)
		c.shapes[id] = sv
		c.drawOrder = append(c.drawOrder, id)
	}
	c.Invalidate()
}

//func (c *canvasView) MouseDown(ev *desktop.MouseEvent) {
//	ctrl := ev.Modifier == fyne.KeyModifierControl
//	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
//	me := mouseEvent{Pos: p, Ctrl: ctrl}
//
//	shape, marker, hitMarker := c.hitHandles(p)
//	if hitMarker {
//		c.current.OnResizeActivated(me, shape, marker)
//		return
//	}
//
//	shape = c.hitTestShape(p)
//	if shape != nil {
//		c.current.OnShapeClick(me, shape, ctrl)
//		return
//	}
//
//	c.current.OnEmptyClick(me, ctrl)
//}
//
//func (c *canvasView) MouseUp(ev *desktop.MouseEvent) {
//	ctrl := ev.Modifier == fyne.KeyModifierControl
//	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
//	c.current.OnMouseUp(mouseEvent{Pos: p, Ctrl: ctrl})
//}
//
//func (c *canvasView) MouseIn(ev *desktop.MouseEvent) {}
//
//func (c *canvasView) MouseMoved(ev *desktop.MouseEvent) {
//	ctrl := ev.Modifier == fyne.KeyModifierControl
//	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
//	c.current.OnMouseMove(mouseEvent{Pos: p, Ctrl: ctrl})
//}

func (c *canvasView) MouseOut() {
	c.current.OnMouseLeave()
}

func (c *canvasView) SetState(s State) {
	c.current = s
}

func (c *canvasView) ClearSelection() {
	c.mv.ClearSelection()
}

func (c *canvasView) DeleteSelection() {
	c.mv.Delete()
}

func (c *canvasView) Invalidate() {
	c.window.Invalidate()
}

func (c *canvasView) hitTestShape(p geometry.Point) ShapeView {
	for i := len(c.drawOrder) - 1; i >= 0; i-- {
		id := c.drawOrder[i]
		sv := c.shapes[id]
		if sv == nil {
			continue
		}
		if sv.IsDeleted() {
			continue
		}
		if sv.Hit(p) {
			return sv
		}
	}
	return nil
}

func (c *canvasView) hitHandles(p geometry.Point) (ShapeView, geometry.ResizeHandle, bool) {
	for i := len(c.drawOrder) - 1; i >= 0; i-- {
		id := c.drawOrder[i]
		sv := c.shapes[id]
		if sv == nil {
			continue
		}
		if sv.IsDeleted() || !sv.IsSelected() {
			continue
		}
		m, ok := sv.HitHandle(p)
		if ok {
			return sv, m, true
		}
	}
	return nil, 0, false
}
