package geometry

import (
	"testing"
)

func pointEq(a, b Point) bool {
	return a.X == b.X && a.Y == b.Y
}

func boundsEq(a, b Bounds) bool {
	return a.Width == b.Width && a.Height == b.Height
}

func TestCalculateScale_FromTopLeft(t *testing.T) {
	startPos := Point{X: 10, Y: 20}
	startBounds := Bounds{Width: 100, Height: 50}

	delta := Vector{X: 0, Y: 0}
	scale := Scale{ScaleX: 2, ScaleY: 3}

	gotPos, gotBounds := CalculateScale(delta, scale, startPos, startBounds)

	wantBounds := Bounds{Width: 200, Height: 150}
	wantPos := startPos

	if !boundsEq(gotBounds, wantBounds) {
		t.Fail()
	}
	if !pointEq(gotPos, wantPos) {
		t.Fail()
	}
}

func TestCalculateScale_FromTopRight(t *testing.T) {
	startPos := Point{X: 10, Y: 20}
	startBounds := Bounds{Width: 100, Height: 50}

	delta := Vector{X: 5, Y: 0}
	scale := Scale{ScaleX: 2, ScaleY: 3}

	gotPos, gotBounds := CalculateScale(delta, scale, startPos, startBounds)

	wantBounds := Bounds{Width: 200, Height: 150}
	fixed := Point{X: startPos.X + startBounds.Width, Y: startPos.Y}
	wantPos := Point{X: fixed.X - wantBounds.Width, Y: fixed.Y}

	if !boundsEq(gotBounds, wantBounds) {
		t.Fail()
	}
	if !pointEq(gotPos, wantPos) {
		t.Fail()
	}
}

func TestCalculateScale_FromBottomLeft(t *testing.T) {
	startPos := Point{X: 10, Y: 20}
	startBounds := Bounds{Width: 100, Height: 50}

	delta := Vector{X: 0, Y: 7}
	scale := Scale{ScaleX: 2, ScaleY: 3}

	gotPos, gotBounds := CalculateScale(delta, scale, startPos, startBounds)

	wantBounds := Bounds{Width: 200, Height: 150}
	fixed := Point{X: startPos.X, Y: startPos.Y + startBounds.Height}
	wantPos := Point{X: fixed.X, Y: fixed.Y - wantBounds.Height}

	if !boundsEq(gotBounds, wantBounds) {
		t.Fail()
	}
	if !pointEq(gotPos, wantPos) {
		t.Fail()
	}
}

func TestCalculateScale_FromBottomRight(t *testing.T) {
	startPos := Point{X: 10, Y: 20}
	startBounds := Bounds{Width: 100, Height: 50}

	delta := Vector{X: 3, Y: 9}
	scale := Scale{ScaleX: 2, ScaleY: 3}

	gotPos, gotBounds := CalculateScale(delta, scale, startPos, startBounds)

	wantBounds := Bounds{Width: 200, Height: 150}
	fixed := Point{X: startPos.X + startBounds.Width, Y: startPos.Y + startBounds.Height}
	wantPos := Point{X: fixed.X - wantBounds.Width, Y: fixed.Y - wantBounds.Height}

	if !boundsEq(gotBounds, wantBounds) {
		t.Fail()
	}
	if !pointEq(gotPos, wantPos) {
		t.Fail()
	}
}

func TestCalculateScale_NoChange(t *testing.T) {
	startPos := Point{X: 10, Y: 20}
	startBounds := Bounds{Width: 100, Height: 50}

	delta := Vector{X: 0, Y: 0}
	scale := Scale{ScaleX: 1, ScaleY: 1}

	gotPos, gotBounds := CalculateScale(delta, scale, startPos, startBounds)

	if !boundsEq(gotBounds, startBounds) {
		t.Fail()
	}
	if !pointEq(gotPos, startPos) {
		t.Fail()
	}
}
