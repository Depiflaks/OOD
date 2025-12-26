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
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/widget"
)

type FileActions struct {
	Open   func()
	Save   func()
	SaveAs func()
}

type ColorActions struct {
	SetFill   func(c color.Color)
	SetStroke func(c color.Color)
}

type WorkspaceView struct {
	app    fyne.App
	window fyne.Window

	toolbar *ToolbarView
	canvas  *CanvasView

	files  FileActions
	colors ColorActions
}

func NewWorkspaceView(
	a fyne.App,
	mv *modelview.WorkspaceModelView,
	files FileActions,
	colors ColorActions,
) *WorkspaceView {
	w := &WorkspaceView{
		app:    a,
		window: a.NewWindow("Vector Editor"),
		files:  files,
		colors: colors,
	}

	w.toolbar = NewToolbarView(w.window, mv.Toolbar(), files, colors)
	w.canvas = NewCanvasView(mv.Canvas())

	content := container.NewBorder(w.toolbar.Object(), nil, nil, nil, w.canvas)
	w.window.SetContent(content)
	w.window.Resize(fyne.NewSize(1024, 768))

	return w
}

func (w *WorkspaceView) Show() {
	w.window.Show()
}

type ToolbarView struct {
	obj fyne.CanvasObject
}

func NewToolbarView(win fyne.Window, mv *modelview.ToolbarModelView, files FileActions, colors ColorActions) *ToolbarView {
	btnRect := widget.NewButton("Rect", func() { mv.NewRectangle() })
	btnEllipse := widget.NewButton("Ellipse", func() { mv.NewEllipse() })
	btnTriangle := widget.NewButton("Triangle", func() { mv.NewTriangle() })

	fillPreview := canvas.NewRectangle(color.Transparent)
	fillPreview.SetMinSize(fyne.NewSize(24, 24))

	strokePreview := canvas.NewRectangle(color.Transparent)
	strokePreview.SetMinSize(fyne.NewSize(24, 24))

	btnFill := widget.NewButton("Fill", func() {
		d := dialog.NewColorPicker("Fill", "Select fill color", func(c color.Color) {
			fillPreview.FillColor = c
			fillPreview.Refresh()
			if colors.SetFill != nil {
				colors.SetFill(c)
			}
		}, win)
		d.Show()
	})

	btnStroke := widget.NewButton("Stroke", func() {
		d := dialog.NewColorPicker("Stroke", "Select stroke color", func(c color.Color) {
			strokePreview.FillColor = c
			strokePreview.Refresh()
			if colors.SetStroke != nil {
				colors.SetStroke(c)
			}
		}, win)
		d.Show()
	})

	btnOpen := widget.NewButton("Open", func() {
		if files.Open != nil {
			files.Open()
		}
	})
	btnSave := widget.NewButton("Save", func() {
		if files.Save != nil {
			files.Save()
		}
	})
	btnSaveAs := widget.NewButton("Save As", func() {
		if files.SaveAs != nil {
			files.SaveAs()
		}
	})

	obj := container.NewHBox(
		btnRect, btnEllipse, btnTriangle,
		widget.NewSeparator(),
		container.NewHBox(btnFill, fillPreview),
		container.NewHBox(btnStroke, strokePreview),
		widget.NewSeparator(),
		btnOpen, btnSave, btnSaveAs,
	)

	return &ToolbarView{obj: obj}
}

func (t *ToolbarView) Object() fyne.CanvasObject {
	return t.obj
}

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

type ViewState interface {
	OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool)
	OnEmptyClick(e mouseEvent, ctrl bool)
	OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker)
	OnMouseMove(e mouseEvent)
	OnMouseUp(e mouseEvent)
}

type CanvasView struct {
	widget.BaseWidget

	mv *modelview.CanvasModelView

	bg color.Color

	shapes    map[model.ShapeId]*ShapeView
	drawOrder []model.ShapeId

	img    *image.RGBA
	raster *canvas.Raster

	idle     *IdleViewState
	selected *SelectedViewState
	dragging *DraggingViewState
	resizing *ResizingViewState
	current  ViewState

	lastSize fyne.Size

	dirty bool
}

func NewCanvasView(mv *modelview.CanvasModelView) *CanvasView {
	c := &CanvasView{
		mv:     mv,
		bg:     color.White,
		shapes: make(map[model.ShapeId]*ShapeView),
		dirty:  true,
	}
	c.ExtendBaseWidget(c)

	c.idle = &IdleViewState{c: c}
	c.selected = &SelectedViewState{c: c}
	c.dragging = &DraggingViewState{c: c}
	c.resizing = &ResizingViewState{c: c}
	c.current = c.idle

	mv.AddObserver(c)

	return c
}

func (c *CanvasView) CreateRenderer() fyne.WidgetRenderer {
	c.ensureBuffer(c.Size())

	c.raster = canvas.NewRaster(func(w, h int) image.Image {
		if c.img == nil || c.img.Bounds().Dx() != w || c.img.Bounds().Dy() != h {
			c.img = image.NewRGBA(image.Rect(0, 0, maxInt(1, w), maxInt(1, h)))
			c.dirty = true
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

func maxInt(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func (c *CanvasView) ensureBuffer(sz fyne.Size) {
	w := int(math.Max(1, float64(sz.Width)))
	h := int(math.Max(1, float64(sz.Height)))
	c.img = image.NewRGBA(image.Rect(0, 0, w, h))
	c.dirty = true
}

func (c *CanvasView) UpdateShapes(ids []model.ShapeId) {
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

func (c *CanvasView) Tapped(ev *fyne.PointEvent) {}

func (c *CanvasView) MouseDown(ev *desktop.MouseEvent) {
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

func (c *CanvasView) MouseUp(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	c.current.OnMouseUp(mouseEvent{Pos: p, Ctrl: ctrl})
}

func (c *CanvasView) MouseMoved(ev *desktop.MouseEvent) {
	ctrl := ev.Modifier == fyne.KeyModifierControl
	p := geometry.Point{X: float64(ev.Position.X), Y: float64(ev.Position.Y)}
	c.current.OnMouseMove(mouseEvent{Pos: p, Ctrl: ctrl})
}

func (c *CanvasView) Dragged(ev *fyne.DragEvent) {}

func (c *CanvasView) DragEnd() {}

func (c *CanvasView) setState(s ViewState) {
	c.current = s
}

func (c *CanvasView) clearSelection() {
	c.mv.ClearSelection()
	c.dirty = true
	c.Refresh()
}

func (c *CanvasView) deleteSelection() {
	c.mv.Delete()
	c.dirty = true
	c.Refresh()
}

func (c *CanvasView) invalidate() {
	c.dirty = true
	c.Refresh()
}

func (c *CanvasView) hitTestShape(p geometry.Point) *ShapeView {
	for i := len(c.drawOrder) - 1; i >= 0; i-- {
		id := c.drawOrder[i]
		sv := c.shapes[id]
		if sv == nil {
			continue
		}
		if sv.Deleted() {
			continue
		}
		if sv.Hit(p) {
			return sv
		}
	}
	return nil
}

func (c *CanvasView) hitTestHandles(p geometry.Point) (*ShapeView, ResizeMarker, bool) {
	for i := len(c.drawOrder) - 1; i >= 0; i-- {
		id := c.drawOrder[i]
		sv := c.shapes[id]
		if sv == nil {
			continue
		}
		if sv.Deleted() || !sv.Selected() {
			continue
		}
		m, ok := sv.HitHandle(p)
		if ok {
			return sv, m, true
		}
	}
	return nil, 0, false
}

func (c *CanvasView) selectionRect() geometry.Rect {
	first := true
	var r geometry.Rect
	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.Deleted() || !sv.Selected() {
			continue
		}
		pos := sv.Position()
		b := sv.Bounds()
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
	c    *CanvasView
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

func (c *CanvasView) redraw() {
	if c.img == nil {
		return
	}

	draw.Draw(c.img, c.img.Bounds(), &image.Uniform{c.bg}, image.Point{}, draw.Src)

	for _, id := range c.drawOrder {
		sv := c.shapes[id]
		if sv == nil || sv.Deleted() {
			continue
		}
		sv.Draw(c.img)
	}

	if c.raster != nil {
		c.raster.Refresh()
	}
}

type ShapeView struct {
	mv     *modelview.ShapeModelView
	canvas *CanvasView

	pos   geometry.Point
	b     geometry.Bounds
	style geometry.Style
	typ   model.ShapeType

	deleted  bool
	selected bool
}

func NewShapeView(mv *modelview.ShapeModelView, canvas *CanvasView) *ShapeView {
	s := &ShapeView{mv: mv, canvas: canvas}
	s.syncFromModel()
	mv.AddObserver(s)
	return s
}

func (s *ShapeView) syncFromModel() {
	s.pos = s.mv.GetPosition()
	s.b = s.mv.GetBounds()
	s.style = s.mv.GetStyle()
	s.typ = s.mv.GetShapeType()
	s.deleted = s.mv.Deleted()
	s.selected = s.mv.IsSelected()
}

func (s *ShapeView) UpdateRect(position geometry.Point, bounds geometry.Bounds) {
	s.pos = position
	s.b = bounds
	s.canvas.invalidate()
}

func (s *ShapeView) UpdateStyle(style geometry.Style) {
	s.style = style
	s.canvas.invalidate()
}

func (s *ShapeView) UpdateDeleted(deleted bool) {
	s.deleted = deleted
	s.canvas.invalidate()
}

func (s *ShapeView) Deleted() bool {
	return s.mv.Deleted()
}

func (s *ShapeView) Selected() bool {
	return s.mv.IsSelected()
}

func (s *ShapeView) Position() geometry.Point {
	return s.mv.GetPosition()
}

func (s *ShapeView) Bounds() geometry.Bounds {
	return s.mv.GetBounds()
}

func (s *ShapeView) Select(ctrl bool) {
	s.mv.Select(ctrl)
	s.canvas.invalidate()
}

func (s *ShapeView) StartDragging() { s.mv.StartDragging() }
func (s *ShapeView) StopDragging()  { s.mv.StopDragging() }
func (s *ShapeView) Drag(delta geometry.Vector) {
	s.mv.Drag(delta)
}

func (s *ShapeView) StartResizing() { s.mv.StartResizing() }
func (s *ShapeView) StopResizing()  { s.mv.StopResizing() }
func (s *ShapeView) Resize(delta geometry.Vector, scale geometry.Scale) {
	s.mv.Resize(delta, scale)
}

func (s *ShapeView) Hit(p geometry.Point) bool {
	pos := s.mv.GetPosition()
	b := s.mv.GetBounds()
	switch s.mv.GetShapeType() {
	case model.Rect:
		return p.X >= pos.X && p.X <= pos.X+b.Width && p.Y >= pos.Y && p.Y <= pos.Y+b.Height
	case model.Ellipse:
		if b.Width == 0 || b.Height == 0 {
			return false
		}
		cx := pos.X + b.Width/2
		cy := pos.Y + b.Height/2
		rx := b.Width / 2
		ry := b.Height / 2
		dx := (p.X - cx) / rx
		dy := (p.Y - cy) / ry
		return dx*dx+dy*dy <= 1.0
	case model.Triangle:
		ax := pos.X + b.Width/2
		ay := pos.Y
		bx := pos.X
		by := pos.Y + b.Height
		cx := pos.X + b.Width
		cy := pos.Y + b.Height
		return pointInTriangle(p.X, p.Y, ax, ay, bx, by, cx, cy)
	default:
		return false
	}
}

func (s *ShapeView) HitHandle(p geometry.Point) (ResizeMarker, bool) {
	handle := s.handleSize()
	pos := s.mv.GetPosition()
	b := s.mv.GetBounds()

	tl := geometry.Rect{Position: pos, Size: geometry.Bounds{Width: handle, Height: handle}}
	tr := geometry.Rect{Position: geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y}, Size: geometry.Bounds{Width: handle, Height: handle}}
	bl := geometry.Rect{Position: geometry.Point{X: pos.X, Y: pos.Y + b.Height - handle}, Size: geometry.Bounds{Width: handle, Height: handle}}
	br := geometry.Rect{Position: geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y + b.Height - handle}, Size: geometry.Bounds{Width: handle, Height: handle}}

	if pointInRect(p, tl) {
		return MarkerTopLeft, true
	}
	if pointInRect(p, tr) {
		return MarkerTopRight, true
	}
	if pointInRect(p, bl) {
		return MarkerBottomLeft, true
	}
	if pointInRect(p, br) {
		return MarkerBottomRight, true
	}
	return 0, false
}

func (s *ShapeView) Draw(img *image.RGBA) {
	s.syncFromModel()
	if s.mv.Deleted() {
		return
	}

	pos := s.pos
	b := s.b

	fill := rgbaFromPtr(s.style.Fill)
	stroke := rgbaFromPtr(s.style.Stroke)

	switch s.typ {
	case model.Rect:
		drawRect(img, pos, b, fill, stroke)
	case model.Ellipse:
		drawEllipse(img, pos, b, fill, stroke)
	case model.Triangle:
		drawTriangle(img, pos, b, fill, stroke)
	}

	if s.mv.IsSelected() {
		drawSelection(img, pos, b)
	}
}

func (s *ShapeView) handleSize() float64 {
	return 8
}

func rgbaFromPtr(p *color.Color) color.RGBA {
	if p == nil || *p == nil {
		return color.RGBA{0, 0, 0, 0}
	}
	c := color.RGBAModel.Convert(*p).(color.RGBA)
	return c
}

func clampi(v, lo, hi int) int {
	if v < lo {
		return lo
	}
	if v > hi {
		return hi
	}
	return v
}

func drawRect(img *image.RGBA, pos geometry.Point, b geometry.Bounds, fill, stroke color.RGBA) {
	x0 := int(math.Round(pos.X))
	y0 := int(math.Round(pos.Y))
	x1 := int(math.Round(pos.X + b.Width))
	y1 := int(math.Round(pos.Y + b.Height))

	x0 = clampi(x0, 0, img.Bounds().Dx())
	y0 = clampi(y0, 0, img.Bounds().Dy())
	x1 = clampi(x1, 0, img.Bounds().Dx())
	y1 = clampi(y1, 0, img.Bounds().Dy())

	if fill.A != 0 && x1 > x0 && y1 > y0 {
		draw.Draw(img, image.Rect(x0, y0, x1, y1), &image.Uniform{fill}, image.Point{}, draw.Over)
	}

	if stroke.A != 0 {
		for x := x0; x < x1; x++ {
			img.SetRGBA(x, y0, stroke)
			if y1-1 >= 0 && y1-1 < img.Bounds().Dy() {
				img.SetRGBA(x, y1-1, stroke)
			}
		}
		for y := y0; y < y1; y++ {
			img.SetRGBA(x0, y, stroke)
			if x1-1 >= 0 && x1-1 < img.Bounds().Dx() {
				img.SetRGBA(x1-1, y, stroke)
			}
		}
	}
}

func drawEllipse(img *image.RGBA, pos geometry.Point, b geometry.Bounds, fill, stroke color.RGBA) {
	if b.Width <= 0 || b.Height <= 0 {
		return
	}

	x0 := int(math.Floor(pos.X))
	y0 := int(math.Floor(pos.Y))
	x1 := int(math.Ceil(pos.X + b.Width))
	y1 := int(math.Ceil(pos.Y + b.Height))

	x0 = clampi(x0, 0, img.Bounds().Dx())
	y0 = clampi(y0, 0, img.Bounds().Dy())
	x1 = clampi(x1, 0, img.Bounds().Dx())
	y1 = clampi(y1, 0, img.Bounds().Dy())

	cx := pos.X + b.Width/2
	cy := pos.Y + b.Height/2
	rx := b.Width / 2
	ry := b.Height / 2

	for y := y0; y < y1; y++ {
		for x := x0; x < x1; x++ {
			px := float64(x) + 0.5
			py := float64(y) + 0.5
			dx := (px - cx) / rx
			dy := (py - cy) / ry
			v := dx*dx + dy*dy
			if fill.A != 0 && v <= 1.0 {
				img.SetRGBA(x, y, fill)
				continue
			}
			if stroke.A != 0 && v <= 1.03 && v >= 0.97 {
				img.SetRGBA(x, y, stroke)
			}
		}
	}
}

func drawTriangle(img *image.RGBA, pos geometry.Point, b geometry.Bounds, fill, stroke color.RGBA) {
	ax := pos.X + b.Width/2
	ay := pos.Y
	bx := pos.X
	by := pos.Y + b.Height
	cx := pos.X + b.Width
	cy := pos.Y + b.Height

	minX := int(math.Floor(math.Min(ax, math.Min(bx, cx))))
	maxX := int(math.Ceil(math.Max(ax, math.Max(bx, cx))))
	minY := int(math.Floor(math.Min(ay, math.Min(by, cy))))
	maxY := int(math.Ceil(math.Max(ay, math.Max(by, cy))))

	minX = clampi(minX, 0, img.Bounds().Dx())
	minY = clampi(minY, 0, img.Bounds().Dy())
	maxX = clampi(maxX, 0, img.Bounds().Dx())
	maxY = clampi(maxY, 0, img.Bounds().Dy())

	for y := minY; y < maxY; y++ {
		for x := minX; x < maxX; x++ {
			px := float64(x) + 0.5
			py := float64(y) + 0.5
			if fill.A != 0 && pointInTriangle(px, py, ax, ay, bx, by, cx, cy) {
				img.SetRGBA(x, y, fill)
			}
		}
	}

	if stroke.A != 0 {
		drawLine(img, ax, ay, bx, by, stroke)
		drawLine(img, bx, by, cx, cy, stroke)
		drawLine(img, cx, cy, ax, ay, stroke)
	}
}

func drawSelection(img *image.RGBA, pos geometry.Point, b geometry.Bounds) {
	stroke := color.RGBA{0, 120, 255, 255}
	handle := 8.0

	drawRect(img, pos, b, color.RGBA{0, 0, 0, 0}, stroke)

	drawRect(img, pos, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
}

func drawLine(img *image.RGBA, x0, y0, x1, y1 float64, c color.RGBA) {
	dx := math.Abs(x1 - x0)
	dy := math.Abs(y1 - y0)
	sx := -1.0
	if x0 < x1 {
		sx = 1.0
	}
	sy := -1.0
	if y0 < y1 {
		sy = 1.0
	}
	err := dx - dy

	x := x0
	y := y0
	for {
		ix := int(math.Round(x))
		iy := int(math.Round(y))
		if ix >= 0 && iy >= 0 && ix < img.Bounds().Dx() && iy < img.Bounds().Dy() {
			img.SetRGBA(ix, iy, c)
		}
		if math.Abs(x-x1) < 0.5 && math.Abs(y-y1) < 0.5 {
			break
		}
		e2 := 2 * err
		if e2 > -dy {
			err -= dy
			x += sx
		}
		if e2 < dx {
			err += dx
			y += sy
		}
	}
}

func pointInRect(p geometry.Point, r geometry.Rect) bool {
	return p.X >= r.Position.X &&
		p.X <= r.Position.X+r.Size.Width &&
		p.Y >= r.Position.Y &&
		p.Y <= r.Position.Y+r.Size.Height
}

func sign(px, py, ax, ay, bx, by float64) float64 {
	return (px-bx)*(ay-by) - (ax-bx)*(py-by)
}

func pointInTriangle(px, py, ax, ay, bx, by, cx, cy float64) bool {
	d1 := sign(px, py, ax, ay, bx, by)
	d2 := sign(px, py, bx, by, cx, cy)
	d3 := sign(px, py, cx, cy, ax, ay)

	hasNeg := (d1 < 0) || (d2 < 0) || (d3 < 0)
	hasPos := (d1 > 0) || (d2 > 0) || (d3 > 0)

	return !(hasNeg && hasPos)
}

type IdleViewState struct {
	c *CanvasView
}

func (s *IdleViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	shape.Select(ctrl)
	s.c.setState(s.c.selected)
	s.c.invalidate()
}

func (s *IdleViewState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.clearSelection()
	s.c.setState(s.c.idle)
}

func (s *IdleViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *IdleViewState) OnMouseMove(e mouseEvent) {}

func (s *IdleViewState) OnMouseUp(e mouseEvent) {}

type SelectedViewState struct {
	c *CanvasView
}

func (s *SelectedViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {
	if !shape.Selected() || ctrl {
		shape.Select(ctrl)
	}
	shape.StartDragging()
	s.c.dragging.Begin(e, shape)
	s.c.setState(s.c.dragging)
	s.c.invalidate()
}

func (s *SelectedViewState) OnEmptyClick(e mouseEvent, ctrl bool) {
	s.c.clearSelection()
	s.c.setState(s.c.idle)
}

func (s *SelectedViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {
	shape.StartResizing()
	s.c.resizing.Begin(e, shape, marker)
	s.c.setState(s.c.resizing)
	s.c.invalidate()
}

func (s *SelectedViewState) OnMouseMove(e mouseEvent) {}

func (s *SelectedViewState) OnMouseUp(e mouseEvent) {}

type DraggingViewState struct {
	c *CanvasView

	active     *ShapeView
	startMouse geometry.Point
}

func (s *DraggingViewState) Begin(e mouseEvent, active *ShapeView) {
	s.active = active
	s.startMouse = e.Pos
}

func (s *DraggingViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *DraggingViewState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *DraggingViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *DraggingViewState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}
	d := geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}
	s.active.Drag(d)
	s.c.invalidate()
}

func (s *DraggingViewState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopDragging()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.invalidate()
}

type ResizingViewState struct {
	c *CanvasView

	active     *ShapeView
	marker     ResizeMarker
	startMouse geometry.Point
	startSel   geometry.Rect
}

func (s *ResizingViewState) Begin(e mouseEvent, active *ShapeView, marker ResizeMarker) {
	s.active = active
	s.marker = marker
	s.startMouse = e.Pos
	s.startSel = s.c.selectionRect()
}

func (s *ResizingViewState) OnShapeClick(e mouseEvent, shape *ShapeView, ctrl bool) {}

func (s *ResizingViewState) OnEmptyClick(e mouseEvent, ctrl bool) {}

func (s *ResizingViewState) OnResizeActivated(e mouseEvent, shape *ShapeView, marker ResizeMarker) {}

func (s *ResizingViewState) OnMouseMove(e mouseEvent) {
	if s.active == nil {
		return
	}

	dm := geometry.Vector{X: e.Pos.X - s.startMouse.X, Y: e.Pos.Y - s.startMouse.Y}

	left := s.startSel.Position.X
	top := s.startSel.Position.Y
	right := s.startSel.Position.X + s.startSel.Size.Width
	bottom := s.startSel.Position.Y + s.startSel.Size.Height

	newLeft := left
	newTop := top
	newRight := right
	newBottom := bottom

	switch s.marker {
	case MarkerTopLeft:
		newLeft = left + dm.X
		newTop = top + dm.Y
	case MarkerTopRight:
		newRight = right + dm.X
		newTop = top + dm.Y
	case MarkerBottomLeft:
		newLeft = left + dm.X
		newBottom = bottom + dm.Y
	case MarkerBottomRight:
		newRight = right + dm.X
		newBottom = bottom + dm.Y
	}

	if newRight-newLeft < 1 {
		newRight = newLeft + 1
	}
	if newBottom-newTop < 1 {
		newBottom = newTop + 1
	}

	newSel := geometry.Rect{
		Position: geometry.Point{X: newLeft, Y: newTop},
		Size:     geometry.Bounds{Width: newRight - newLeft, Height: newBottom - newTop},
	}

	scaleX := newSel.Size.Width / s.startSel.Size.Width
	scaleY := newSel.Size.Height / s.startSel.Size.Height
	scale := geometry.Scale{ScaleX: scaleX, ScaleY: scaleY}

	deltaTL := geometry.Vector{
		X: newSel.Position.X - s.startSel.Position.X,
		Y: newSel.Position.Y - s.startSel.Position.Y,
	}

	s.active.Resize(deltaTL, scale)
	s.c.invalidate()
}

func (s *ResizingViewState) OnMouseUp(e mouseEvent) {
	if s.active != nil {
		s.active.StopResizing()
	}
	s.active = nil
	s.c.setState(s.c.selected)
	s.c.invalidate()
}
