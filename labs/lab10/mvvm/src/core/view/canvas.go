package view

import (
	"image/color"

	"gioui.org/app"
	"gioui.org/io/event"
	"gioui.org/io/pointer"
	"gioui.org/layout"
	"gioui.org/op/clip"
	"gioui.org/op/paint"

	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"
)

type mouseEvent struct {
	pos  geometry.Point
	ctrl bool
}

type State interface {
	OnShapeClick(e mouseEvent, shape ShapeView)
	OnEmptyClick()
	OnResizeActivated(e mouseEvent, shape ShapeView, marker geometry.ResizeHandle)
	OnMouseMove(e mouseEvent)
	OnMouseUp(e mouseEvent)
	OnMouseLeave()
}

type stateMachine interface {
	CurrentState() State
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

func (c *canvasView) CurrentState() State {
	return c.current
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
	c.processBackground(gtx)

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() {
			continue
		}
		sv.Process(gtx)
	}

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() || !sv.IsSelected() {
			continue
		}
		sv.ProcessHandles(gtx)
	}

	return layout.Dimensions{Size: gtx.Constraints.Max}
}

func (c *canvasView) processBackground(gtx layout.Context) {
	background := c.mv.GetBackgroundColor()
	st := clip.Rect{Max: gtx.Constraints.Max}.Push(gtx.Ops)
	col := color.NRGBAModel.Convert(background).(color.NRGBA)
	paint.Fill(gtx.Ops, col)

	event.Op(gtx.Ops, c)

	for {
		ev, ok := gtx.Event(pointer.Filter{
			Target: c,
			Kinds:  pointer.Press,
		})
		if !ok {
			break
		}
		pointerEv, ok := ev.(pointer.Event)
		if !ok {
			break
		}
		switch pointerEv.Kind {
		case pointer.Press:
			c.current.OnEmptyClick()
		default:
			panic("something goes wrong")
		}
	}
	st.Pop()
}

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
//	shape, marker, hitMarker := c.hitHandles(p)
//	if hitMarker {
//		c.current.OnResizeActivated(me, shape, marker)
//		return
//	}
//
//}

//func (c *canvasView) MouseMoved(ev *desktop.MouseEvent) {
//	ctrl := ev.Modifier == fyne.KeyModifierControl
//	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
//	c.current.OnMouseMove(mouseEvent{pos: p, ctrl: ctrl})
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
