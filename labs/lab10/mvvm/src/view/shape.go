package view

import (
	"image"
	"image/color"
	"image/draw"
	"io"
	"math"
	"vector-editor/src/file"

	"vector-editor/src/geometry"
	"vector-editor/src/model"
	"vector-editor/src/modelview"

	xdraw "golang.org/x/image/draw"
)

type ShapeView struct {
	mv     modelview.ShapeModelView
	canvas CanvasView
}

func NewShapeView(mv modelview.ShapeModelView, canvas CanvasView) *ShapeView {
	s := &ShapeView{mv: mv, canvas: canvas}
	mv.AddObserver(s)
	return s
}

func (s *ShapeView) Update() {
	s.canvas.Invalidate()
}

func (s *ShapeView) IsDeleted() bool {
	return s.mv.IsDeleted()
}

func (s *ShapeView) IsSelected() bool {
	return s.mv.IsSelected()
}

func (s *ShapeView) GetPosition() geometry.Point {
	return s.mv.GetPosition()
}

func (s *ShapeView) GetBounds() geometry.Bounds {
	return s.mv.GetBounds()
}

func (s *ShapeView) Select(ctrl bool) {
	s.mv.Select(ctrl)
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
	if s.mv.IsDeleted() {
		return
	}

	pos := s.mv.GetPosition()
	b := s.mv.GetBounds()

	fill := rgbaFromPtr(s.mv.GetStyle().Fill)
	stroke := rgbaFromPtr(s.mv.GetStyle().Stroke)

	switch s.mv.GetShapeType() {
	case model.Rect:
		if s.mv.GetStyle().Image == nil {
			drawRect(img, pos, b, fill, stroke)
		} else {
			drawRectImage(
				img,
				pos,
				b,
				file.Open(*s.mv.GetStyle().Image),
			)
		}
	case model.Ellipse:
		drawEllipse(img, pos, b, fill, stroke)
	case model.Triangle:
		drawTriangle(img, pos, b, fill, stroke)
	}
}

func (s *ShapeView) DrawSelection(img *image.RGBA) {
	stroke := color.RGBA{0, 120, 255, 255}
	handle := 8.0
	pos := s.mv.GetPosition()
	b := s.mv.GetBounds()
	drawRect(img, pos, b, color.RGBA{0, 0, 0, 0}, stroke)

	drawRect(img, pos, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
	drawRect(img, geometry.Point{X: pos.X + b.Width - handle, Y: pos.Y + b.Height - handle}, geometry.Bounds{Width: handle, Height: handle}, stroke, stroke)
}

func (s *ShapeView) handleSize() float64 {
	return 8
}

func rgbaFromPtr(p *color.Color) color.RGBA {
	if p == nil || *p == nil {
		return color.RGBA{}
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

func drawRect(
	img *image.RGBA,
	pos geometry.Point,
	b geometry.Bounds,
	fill,
	stroke color.RGBA,
) {
	x0, y0, x1, y1 := getShapeBounds(img, pos, b)

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

func drawRectImage(
	imgDst *image.RGBA,
	pos geometry.Point,
	b geometry.Bounds,
	rc io.ReadCloser,
) {
	if imgDst == nil || rc == nil {
		if rc != nil {
			_ = rc.Close()
		}
		return
	}
	defer func() { _ = rc.Close() }()

	srcImg, _, err := image.Decode(rc)
	if err != nil || srcImg == nil {
		return
	}

	x0, y0, x1, y1 := getShapeBounds(imgDst, pos, b)
	if x1 <= x0 || y1 <= y0 {
		return
	}

	dstRectWorld := image.Rect(x0, y0, x1, y1)
	sb := srcImg.Bounds()
	sw, sh := sb.Dx(), sb.Dy()
	if sw <= 0 || sh <= 0 {
		return
	}

	dw, dh := dstRectWorld.Dx(), dstRectWorld.Dy()
	if dw <= 0 || dh <= 0 {
		return
	}

	targetWorld := dstRectWorld
	targetClip, srcSub, ok := clipDstAndMapSrc(targetWorld, imgDst.Bounds(), sb)
	if !ok {
		return
	}

	xdraw.CatmullRom.Scale(imgDst, targetClip, srcImg, srcSub, draw.Over, nil)
}

func clipDstAndMapSrc(dstWorld, dstBounds, srcBounds image.Rectangle) (dstClip image.Rectangle, srcSub image.Rectangle, ok bool) {
	dstClip = dstWorld.Intersect(dstBounds)
	if dstClip.Empty() {
		return image.Rectangle{}, image.Rectangle{}, false
	}

	w := float64(dstWorld.Dx())
	h := float64(dstWorld.Dy())
	if w <= 0 || h <= 0 {
		return image.Rectangle{}, image.Rectangle{}, false
	}

	leftFrac := float64(dstClip.Min.X-dstWorld.Min.X) / w
	rightFrac := float64(dstWorld.Max.X-dstClip.Max.X) / w
	topFrac := float64(dstClip.Min.Y-dstWorld.Min.Y) / h
	botFrac := float64(dstWorld.Max.Y-dstClip.Max.Y) / h

	sx0 := float64(srcBounds.Min.X) + leftFrac*float64(srcBounds.Dx())
	sx1 := float64(srcBounds.Max.X) - rightFrac*float64(srcBounds.Dx())
	sy0 := float64(srcBounds.Min.Y) + topFrac*float64(srcBounds.Dy())
	sy1 := float64(srcBounds.Max.Y) - botFrac*float64(srcBounds.Dy())

	x0 := int(math.Floor(sx0))
	y0 := int(math.Floor(sy0))
	x1 := int(math.Ceil(sx1))
	y1 := int(math.Ceil(sy1))

	srcSub = image.Rect(x0, y0, x1, y1).Intersect(srcBounds)
	if srcSub.Empty() {
		return image.Rectangle{}, image.Rectangle{}, false
	}

	return dstClip, srcSub, true
}

func drawEllipse(img *image.RGBA, pos geometry.Point, b geometry.Bounds, fill, stroke color.RGBA) {
	if b.Width <= 0 || b.Height <= 0 {
		return
	}
	x0, y0, x1, y1 := getShapeBounds(img, pos, b)

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

func getShapeBounds(img *image.RGBA, pos geometry.Point, b geometry.Bounds) (int, int, int, int) {
	x0 := int(math.Floor(pos.X))
	y0 := int(math.Floor(pos.Y))
	x1 := int(math.Ceil(pos.X + b.Width))
	y1 := int(math.Ceil(pos.Y + b.Height))

	x0 = clampi(x0, 0, img.Bounds().Dx())
	y0 = clampi(y0, 0, img.Bounds().Dy())
	x1 = clampi(x1, 0, img.Bounds().Dx())
	y1 = clampi(y1, 0, img.Bounds().Dy())
	return x0, y0, x1, y1
}

func drawTriangle(img *image.RGBA, pos geometry.Point, b geometry.Bounds, fill, stroke color.RGBA) {
	x, y := pos.X, pos.Y
	w, h := b.Width, b.Height

	if w < 0 {
		x += w
		w = -w
	}
	if h < 0 {
		y += h
		h = -h
	}

	if math.IsNaN(x) || math.IsInf(x, 0) ||
		math.IsNaN(y) || math.IsInf(y, 0) ||
		math.IsNaN(w) || math.IsInf(w, 0) ||
		math.IsNaN(h) || math.IsInf(h, 0) {
		return
	}

	ax := x + w/2
	ay := y
	bx := x
	by := y + h
	cx := x + w
	cy := y + h

	minX := int(math.Floor(math.Min(ax, math.Min(bx, cx))))
	maxX := int(math.Ceil(math.Max(ax, math.Max(bx, cx))))
	minY := int(math.Floor(math.Min(ay, math.Min(by, cy))))
	maxY := int(math.Ceil(math.Max(ay, math.Max(by, cy))))

	minX = clampi(minX, 0, img.Bounds().Dx())
	minY = clampi(minY, 0, img.Bounds().Dy())
	maxX = clampi(maxX, 0, img.Bounds().Dx())
	maxY = clampi(maxY, 0, img.Bounds().Dy())

	const maxPixels = 5_000_000
	if (maxX-minX)*(maxY-minY) > maxPixels {
		return
	}

	for yy := minY; yy < maxY; yy++ {
		for xx := minX; xx < maxX; xx++ {
			px := float64(xx) + 0.5
			py := float64(yy) + 0.5
			if fill.A != 0 && pointInTriangle(px, py, ax, ay, bx, by, cx, cy) {
				img.SetRGBA(xx, yy, fill)
			}
		}
	}

	if stroke.A != 0 {
		drawLine(img, ax, ay, bx, by, stroke)
		drawLine(img, bx, by, cx, cy, stroke)
		drawLine(img, cx, cy, ax, ay, stroke)
	}
}

func drawLine(img *image.RGBA, x0, y0, x1, y1 float64, c color.RGBA) {
	if math.IsNaN(x0) || math.IsInf(x0, 0) ||
		math.IsNaN(y0) || math.IsInf(y0, 0) ||
		math.IsNaN(x1) || math.IsInf(x1, 0) ||
		math.IsNaN(y1) || math.IsInf(y1, 0) {
		return
	}

	w := float64(img.Bounds().Dx() - 1)
	h := float64(img.Bounds().Dy() - 1)
	if w < 0 || h < 0 {
		return
	}

	const (
		inside = 0
		left   = 1
		right  = 2
		bottom = 4
		top    = 8
	)

	outCode := func(x, y float64) int {
		code := inside
		if x < 0 {
			code |= left
		} else if x > w {
			code |= right
		}
		if y < 0 {
			code |= top
		} else if y > h {
			code |= bottom
		}
		return code
	}

	code0 := outCode(x0, y0)
	code1 := outCode(x1, y1)

	for {
		if (code0 | code1) == 0 {
			break
		}
		if (code0 & code1) != 0 {
			return
		}

		var codeOut int
		var x, y float64
		if code0 != 0 {
			codeOut = code0
		} else {
			codeOut = code1
		}

		dx := x1 - x0
		dy := y1 - y0

		if (codeOut & top) != 0 {
			if dy == 0 {
				return
			}
			x = x0 + dx*(0-y0)/dy
			y = 0
		} else if (codeOut & bottom) != 0 {
			if dy == 0 {
				return
			}
			x = x0 + dx*(h-y0)/dy
			y = h
		} else if (codeOut & right) != 0 {
			if dx == 0 {
				return
			}
			y = y0 + dy*(w-x0)/dx
			x = w
		} else {
			if dx == 0 {
				return
			}
			y = y0 + dy*(0-x0)/dx
			x = 0
		}

		if codeOut == code0 {
			x0, y0 = x, y
			code0 = outCode(x0, y0)
		} else {
			x1, y1 = x, y
			code1 = outCode(x1, y1)
		}
	}

	x0i := int(math.Round(x0))
	y0i := int(math.Round(y0))
	x1i := int(math.Round(x1))
	y1i := int(math.Round(y1))

	dx := absInt(x1i - x0i)
	dy := -absInt(y1i - y0i)
	sx := 1
	if x0i > x1i {
		sx = -1
	}
	sy := 1
	if y0i > y1i {
		sy = -1
	}
	err := dx + dy

	for {
		if x0i >= 0 && y0i >= 0 && x0i < img.Bounds().Dx() && y0i < img.Bounds().Dy() {
			img.SetRGBA(x0i, y0i, c)
		}
		if x0i == x1i && y0i == y1i {
			break
		}
		e2 := 2 * err
		if e2 >= dy {
			err += dy
			x0i += sx
		}
		if e2 <= dx {
			err += dx
			y0i += sy
		}
	}
}

func absInt(x int) int {
	if x < 0 {
		return -x
	}
	return x
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
