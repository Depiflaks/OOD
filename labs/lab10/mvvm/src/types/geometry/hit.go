package geometry

type ResizeHandle int

const (
	HandleTopLeft ResizeHandle = iota
	HandleTopRight
	HandleBottomLeft
	HandleBottomRight
)

//func HitHandle(
//	p Point,
//	position Point,
//	bounds Bounds,
//	handleSize float64,
//) (ResizeHandle, bool) {
//	tl := Rect{Position: position, Size: Bounds{Width: handleSize, Height: handleSize}}
//	tr := Rect{Position: Point{X: position.X + bounds.Width - handleSize, Y: position.Y}, Size: Bounds{Width: handleSize, Height: handleSize}}
//	bl := Rect{Position: Point{X: position.X, Y: position.Y + bounds.Height - handleSize}, Size: Bounds{Width: handleSize, Height: handleSize}}
//	br := Rect{Position: Point{X: position.X + bounds.Width - handleSize, Y: position.Y + bounds.Height - handleSize}, Size: Bounds{Width: handleSize, Height: handleSize}}
//
//	if pointInRect(p, tl) {
//		return HandleTopLeft, true
//	}
//	if pointInRect(p, tr) {
//		return HandleTopRight, true
//	}
//	if pointInRect(p, bl) {
//		return HandleBottomLeft, true
//	}
//	if pointInRect(p, br) {
//		return HandleBottomRight, true
//	}
//	return 0, false
//}
//
//func HitShape(
//	p Point,
//	position Point,
//	bounds Bounds,
//	shapeType types.ShapeType,
//) bool {
//	switch shapeType {
//	case types.Rect:
//		return p.X >= position.X && p.X <= position.X+bounds.Width && p.Y >= position.Y && p.Y <= position.Y+bounds.Height
//	case types.Ellipse:
//		if bounds.Width == 0 || bounds.Height == 0 {
//			return false
//		}
//		cx := position.X + bounds.Width/2
//		cy := position.Y + bounds.Height/2
//		rx := bounds.Width / 2
//		ry := bounds.Height / 2
//		dx := (p.X - cx) / rx
//		dy := (p.Y - cy) / ry
//		return dx*dx+dy*dy <= 1.0
//	case types.Triangle:
//		ax := position.X + bounds.Width/2
//		ay := position.Y
//		bx := position.X
//		by := position.Y + bounds.Height
//		cx := position.X + bounds.Width
//		cy := position.Y + bounds.Height
//		return PointInTriangle(p.X, p.Y, ax, ay, bx, by, cx, cy)
//	default:
//		return false
//	}
//}
//
//func pointInRect(p Point, r Rect) bool {
//	return p.X >= r.Position.X &&
//		p.X <= r.Position.X+r.Size.Width &&
//		p.Y >= r.Position.Y &&
//		p.Y <= r.Position.Y+r.Size.Height
//}
//
//func PointInTriangle(px, py, ax, ay, bx, by, cx, cy float64) bool {
//	d1 := sign(px, py, ax, ay, bx, by)
//	d2 := sign(px, py, bx, by, cx, cy)
//	d3 := sign(px, py, cx, cy, ax, ay)
//
//	hasNeg := (d1 < 0) || (d2 < 0) || (d3 < 0)
//	hasPos := (d1 > 0) || (d2 > 0) || (d3 > 0)
//
//	return !(hasNeg && hasPos)
//}
//
//func sign(px, py, ax, ay, bx, by float64) float64 {
//	return (px-bx)*(ay-by) - (ax-bx)*(py-by)
//}
