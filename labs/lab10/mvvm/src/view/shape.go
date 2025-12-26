package view

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"math"

	"vector-editor/src/geometry"
	"vector-editor/src/model"
	"vector-editor/src/modelview"
)

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
