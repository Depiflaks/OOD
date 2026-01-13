package view

import (
	"image"
	"image/draw"
	"math"
	"sync"
	"vector-editor/src/core/modelview"
	"vector-editor/src/types"
	"vector-editor/src/types/geometry"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/widget"
)

type mouseEvent struct {
	Pos  geometry.Point
	Ctrl bool
}

type State interface {
	OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool)
	OnEmptyClick(e mouseEvent, ctrl bool)
	OnResizeActivated(e mouseEvent, shape *ShapeView, marker geometry.ResizeHandle)
	OnMouseMove(e mouseEvent)
	OnMouseUp(e mouseEvent)
	OnMouseLeave()
}

type CanvasView interface {
	fyne.CanvasObject

	SetIdleState()
	SetDraggingState(e mouseEvent, active *ShapeView)
	SetResizingState(e mouseEvent, active *ShapeView, marker geometry.ResizeHandle)
	ClearSelection()
	DeleteSelection()
	Invalidate()
}

type canvasView struct {
	widget.BaseWidget

	mv modelview.CanvasModelView

	shapes    map[types.ShapeId]*ShapeView
	drawOrder []types.ShapeId

	img    *image.RGBA
	raster *canvas.Raster

	current State

	lastSize fyne.Size

	dirty   bool
	refresh bool
	mu      sync.Mutex
}

func (c *canvasView) OnBackgroundChanged() {
	c.Invalidate()
}

func (c *canvasView) SetIdleState() {
	c.current = NewIdleState(c)
}

func (c *canvasView) SetDraggingState(e mouseEvent, active *ShapeView) {
	c.current = NewDraggingState(c, e, active)
}

func (c *canvasView) SetResizingState(e mouseEvent, active *ShapeView, marker geometry.ResizeHandle) {
	c.current = NewResizingState(c, e, active, marker)
}

func NewCanvasView(mv modelview.CanvasModelView) CanvasView {
	c := &canvasView{
		mv:     mv,
		shapes: make(map[types.ShapeId]*ShapeView),
		dirty:  true,
	}
	c.ExtendBaseWidget(c)

	c.current = NewIdleState(c)

	mv.AddObserver(c)

	return c
}

func (c *canvasView) CreateRenderer() fyne.WidgetRenderer {
	c.raster = canvas.NewRaster(func(w, h int) image.Image {
		if c.img == nil || c.img.Bounds().Dx() != w || c.img.Bounds().Dy() != h {
			c.ensureBuffer(c.Size())
		}
		if c.dirty {
			c.redraw()
			c.dirty = false
		}
		return c.img
	})

	objs := []fyne.CanvasObject{c.raster}
	return &canvasViewRenderer{c: c, objs: objs}
}

func (c *canvasView) ensureBuffer(sz fyne.Size) {
	w := int(math.Max(1, float64(sz.Width)))
	h := int(math.Max(1, float64(sz.Height)))
	c.img = image.NewRGBA(image.Rect(0, 0, w, h))
	c.dirty = true
}

func (c *canvasView) redraw() {
	if c.img == nil {
		return
	}

	draw.Draw(c.img, c.img.Bounds(), &image.Uniform{C: c.mv.GetBackgroundColor()}, image.Point{}, draw.Src)

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() {
			continue
		}
		sv.Draw(c.img)
	}

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() || !sv.IsSelected() {
			continue
		}
		sv.DrawSelection(c.img)
	}
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

func (c *canvasView) MouseDown(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	me := mouseEvent{Pos: p, Ctrl: ctrl}

	shape, marker, hitMarker := c.hitHandles(p)
	if hitMarker {
		c.current.OnResizeActivated(me, shape, marker)
		return
	}

	shape = c.hitTestShape(p)
	if shape != nil {
		c.current.OnShapeClick(me, shape, ctrl)
		return
	}

	c.current.OnEmptyClick(me, ctrl)
}

func (c *canvasView) MouseUp(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	c.current.OnMouseUp(mouseEvent{Pos: p, Ctrl: ctrl})
}

func (c *canvasView) MouseIn(ev *desktop.MouseEvent) {}

func (c *canvasView) MouseMoved(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	c.current.OnMouseMove(mouseEvent{Pos: p, Ctrl: ctrl})
}

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
	c.mu.Lock()
	if c.dirty {
		c.mu.Unlock()
		return
	}
	c.dirty = true
	c.mu.Unlock()

	c.Refresh()
}

func (c *canvasView) hitTestShape(p geometry.Point) *ShapeView {
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

func (c *canvasView) hitHandles(p geometry.Point) (*ShapeView, geometry.ResizeHandle, bool) {
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

type canvasViewRenderer struct {
	c    *canvasView
	objs []fyne.CanvasObject
}

func (r *canvasViewRenderer) Layout(sz fyne.Size) {
	if r.c.lastSize != sz {
		r.c.lastSize = sz
		r.c.dirty = true
	}
	r.objs[0].Resize(sz)
}

func (r *canvasViewRenderer) MinSize() fyne.Size {
	return fyne.NewSize(64, 64)
}

func (r *canvasViewRenderer) Refresh() {
	r.c.dirty = true
	r.objs[0].Refresh()
}

func (r *canvasViewRenderer) Objects() []fyne.CanvasObject {
	return r.objs
}

func (r *canvasViewRenderer) Destroy() {}
