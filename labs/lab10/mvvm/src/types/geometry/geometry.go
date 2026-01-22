package geometry

type Vector struct {
	X int
	Y int
}

type Point struct {
	X int
	Y int
}

type Rect struct {
	Position Point
	Size     Bounds
}

type Scale struct {
	ScaleX float64
	ScaleY float64
}

type Bounds struct {
	Width  int
	Height int
}

func CalculateScale(
	delta Vector,
	scale Scale,
	startPos Point,
	startBounds Bounds,
) (point Point, bounds Bounds) {
	newBounds := Bounds{
		Width:  int(float64(startBounds.Width) * scale.ScaleX),
		Height: int(float64(startBounds.Height) * scale.ScaleY),
	}

	dx0 := delta.X == 0
	dy0 := delta.Y == 0

	fixedTop := dy0
	fixedLeft := dx0

	var fixed Point
	switch {
	case fixedLeft && fixedTop:
		fixed = Point{X: startPos.X, Y: startPos.Y}
	case !fixedLeft && fixedTop:
		fixed = Point{X: startPos.X + startBounds.Width, Y: startPos.Y}
	case fixedLeft && !fixedTop:
		fixed = Point{X: startPos.X, Y: startPos.Y + startBounds.Height}
	default:
		fixed = Point{X: startPos.X + startBounds.Width, Y: startPos.Y + startBounds.Height}
	}

	var newPos Point
	switch {
	case fixedLeft && fixedTop:
		newPos = Point{X: fixed.X, Y: fixed.Y}
	case !fixedLeft && fixedTop:
		newPos = Point{X: fixed.X - newBounds.Width, Y: fixed.Y}
	case fixedLeft && !fixedTop:
		newPos = Point{X: fixed.X, Y: fixed.Y - newBounds.Height}
	default:
		newPos = Point{X: fixed.X - newBounds.Width, Y: fixed.Y - newBounds.Height}
	}

	return newPos, newBounds
}
