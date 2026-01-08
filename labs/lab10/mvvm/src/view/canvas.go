package view

import (
	"image"
	"image/color"
	"image/draw"
	"math"

	"vector-editor/src/geometry"
	"vector-editor/src/model"
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/widget"
)

type ResizeMarker int

const (
	MarkerTopLeft ResizeMarker = iota
	MarkerTopRight
	MarkerBottomLeft
	MarkerBottomRight
)

type mouseEvent struct {
	Pos  geometry.Point
	Ctrl bool
}

type State interface {
	OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool)
	OnEmptyClick(e mouseEvent, ctrl bool)
	OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker)
	OnMouseMove(e mouseEvent)
	OnMouseUp(e mouseEvent)
}

type CanvasView interface {
	fyne.CanvasObject

	SetIdleState()
	SetDraggingState(e mouseEvent, active *ShapeView)
	SetResizingState(e mouseEvent, active *ShapeView, marker ResizeMarker)
	ClearSelection()
	DeleteSelection()
	Invalidate()
	SelectionRect() geometry.Rect
}

type canvasView struct {
	widget.BaseWidget

	mv modelview.CanvasModelView

	bg color.Color

	shapes    map[model.ShapeId]*ShapeView
	drawOrder []model.ShapeId

	img    *image.RGBA
	raster *canvas.Raster

	current State

	lastSize fyne.Size

	dirty bool
}

func (c *canvasView) SetIdleState() {
	c.current = NewIdleState(c)
}

func (c *canvasView) SetDraggingState(e mouseEvent, active *ShapeView) {
	c.current = NewDraggingState(c, e, active)
}

func (c *canvasView) SetResizingState(e mouseEvent, active *ShapeView, marker ResizeMarker) {
	c.current = NewResizingState(c, e, active, marker)
}

func NewCanvasView(mv modelview.CanvasModelView) CanvasView {
	c := &canvasView{
		mv:     mv,
		bg:     color.White,
		shapes: make(map[model.ShapeId]*ShapeView),
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

	draw.Draw(c.img, c.img.Bounds(), &image.Uniform{C: c.bg}, image.Point{}, draw.Src)

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() {
			// TODO: почему nil?
			continue
		}
		sv.Draw(c.img)
	}
}

func (c *canvasView) OnShapesChanged(ids []model.ShapeId) {
	// TODO: что за хуета здесь творится?
	for _, id := range ids {
		if _, ok := c.shapes[id]; ok {
			continue
		}
		sm := c.mv.GetShape(id)
		if sm == nil {
			continue
		}
		sv := NewShapeView(sm, c)
		c.shapes[id] = sv
		c.drawOrder = append(c.drawOrder, id)
	}
	c.dirty = true
	c.Refresh()
}

func (c *canvasView) MouseDown(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	me := mouseEvent{Pos: p, Ctrl: ctrl}

	shape, marker, hitMarker := c.hitTestHandles(p)
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

func (c *canvasView) MouseOut() {}

func (c *canvasView) SetState(s State) {
	c.current = s
}

func (c *canvasView) ClearSelection() {
	c.mv.ClearSelection()
	// TODO: здесь не должно быть обновления
	c.dirty = true
	c.Refresh()
}

func (c *canvasView) DeleteSelection() {
	c.mv.Delete()
	c.dirty = true
	c.Refresh()
}

func (c *canvasView) Invalidate() {
	c.dirty = true
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

func (c *canvasView) hitTestHandles(p geometry.Point) (*ShapeView, ResizeMarker, bool) {
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

func (c *canvasView) SelectionRect() geometry.Rect {
	first := true
	var r geometry.Rect
	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.IsDeleted() || !sv.IsSelected() {
			continue
		}
		pos := sv.GetPosition()
		b := sv.GetBounds()
		if first {
			r = geometry.Rect{Position: pos, Size: b}
			first = false
			continue
		}
		minX := math.Min(r.Position.X, pos.X)
		minY := math.Min(r.Position.Y, pos.Y)
		maxX := math.Max(r.Position.X+r.Size.Width, pos.X+b.Width)
		maxY := math.Max(r.Position.Y+r.Size.Height, pos.Y+b.Height)
		r.Position = geometry.Point{X: minX, Y: minY}
		r.Size = geometry.Bounds{Width: maxX - minX, Height: maxY - minY}
	}
	return r
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
