package model

import (
	"image/color"
	"testing"
	"vector-editor/graphics"
)

type mockObserver struct {
	updateRectCount  int
	updateStyleCount int

	lastPos    graphics.Point
	lastBounds graphics.Bounds
	lastStyle  graphics.Style
}

func (m *mockObserver) UpdateRect(position graphics.Point, bounds graphics.Bounds) {
	m.updateRectCount++
	m.lastPos = position
	m.lastBounds = bounds
}

func (m *mockObserver) UpdateStyle(style graphics.Style) {
	m.updateStyleCount++
	m.lastStyle = style
}

func makeColorPtr(r, g, b, a uint8) *color.Color {
	var c color.Color = color.RGBA{R: r, G: g, B: b, A: a}
	return &c
}

func makeStyle(fill, stroke *color.Color) graphics.Style {
	return graphics.Style{Fill: fill, Stroke: stroke}
}

func TestWithPosition_AllFields(t *testing.T) {
	s := NewShape(Rect, 7)

	p := graphics.Point{X: 10, Y: 20}
	WithPosition(p)(s)

	if s.id != 7 {
		t.Fail()
	}
	if s.shapeType != Rect {
		t.Fail()
	}
	if s.position != p {
		t.Fail()
	}
	if s.size != (graphics.Bounds{}) {
		t.Fail()
	}
	if s.style != (graphics.Style{}) {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestWithBounds_AllFields(t *testing.T) {
	s := NewShape(Ellipse, 9)

	b := graphics.Bounds{Width: 30, Height: 40}
	WithBounds(b)(s)

	if s.id != 9 {
		t.Fail()
	}
	if s.shapeType != Ellipse {
		t.Fail()
	}
	if s.position != (graphics.Point{}) {
		t.Fail()
	}
	if s.size != b {
		t.Fail()
	}
	if s.style != (graphics.Style{}) {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestMove_AllFields(t *testing.T) {
	s := NewShape(Triangle, 11)

	p0 := graphics.Point{X: 1, Y: 2}
	b0 := graphics.Bounds{Width: 3, Height: 4}
	f0 := makeColorPtr(1, 2, 3, 4)
	st0 := makeColorPtr(5, 6, 7, 8)

	WithPosition(p0)(s)
	WithBounds(b0)(s)
	s.SetStyle(makeStyle(f0, st0))

	d := graphics.Vector{X: 10, Y: -5}
	s.Move(d)

	if s.id != 11 {
		t.Fail()
	}
	if s.shapeType != Triangle {
		t.Fail()
	}
	if s.position != (graphics.Point{X: 11, Y: -3}) {
		t.Fail()
	}
	if s.size != b0 {
		t.Fail()
	}
	if s.style.Fill != f0 || s.style.Stroke != st0 {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestResize_AllFields(t *testing.T) {
	s := NewShape(Rect, 12)

	p0 := graphics.Point{X: 7, Y: 8}
	b0 := graphics.Bounds{Width: 9, Height: 10}
	f0 := makeColorPtr(10, 20, 30, 40)
	st0 := makeColorPtr(50, 60, 70, 80)

	WithPosition(p0)(s)
	WithBounds(b0)(s)
	s.SetStyle(makeStyle(f0, st0))

	b1 := graphics.Bounds{Width: 100, Height: 200}
	s.Resize(b1)

	if s.id != 12 {
		t.Fail()
	}
	if s.shapeType != Rect {
		t.Fail()
	}
	if s.position != p0 {
		t.Fail()
	}
	if s.size != b1 {
		t.Fail()
	}
	if s.style.Fill != f0 || s.style.Stroke != st0 {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestUpdateRect_AllFields(t *testing.T) {
	s := NewShape(Ellipse, 13)

	p0 := graphics.Point{X: 1, Y: 1}
	b0 := graphics.Bounds{Width: 2, Height: 2}
	f0 := makeColorPtr(1, 1, 1, 255)
	st0 := makeColorPtr(2, 2, 2, 255)

	WithPosition(p0)(s)
	WithBounds(b0)(s)
	s.SetStyle(makeStyle(f0, st0))

	d := graphics.Vector{X: 3, Y: 4}
	b1 := graphics.Bounds{Width: 10, Height: 20}
	s.UpdateRect(d, b1)

	if s.id != 13 {
		t.Fail()
	}
	if s.shapeType != Ellipse {
		t.Fail()
	}
	if s.position != (graphics.Point{X: 4, Y: 5}) {
		t.Fail()
	}
	if s.size != b1 {
		t.Fail()
	}
	if s.style.Fill != f0 || s.style.Stroke != st0 {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestSetStyle_FillAndStroke(t *testing.T) {
	s := NewShape(Rect, 1)

	f0 := makeColorPtr(1, 2, 3, 4)
	st0 := makeColorPtr(5, 6, 7, 8)
	s.SetStyle(makeStyle(f0, st0))

	if s.style.Fill != f0 || s.style.Stroke != st0 {
		t.Fail()
	}

	f1 := makeColorPtr(9, 10, 11, 12)
	st1 := makeColorPtr(13, 14, 15, 16)
	s.SetStyle(makeStyle(f1, st1))

	if s.style.Fill != f1 || s.style.Stroke != st1 {
		t.Fail()
	}
}

func TestSetStyle_NilFillKeepsOld(t *testing.T) {
	s := NewShape(Rect, 1)

	f0 := makeColorPtr(1, 2, 3, 4)
	st0 := makeColorPtr(5, 6, 7, 8)
	s.SetStyle(makeStyle(f0, st0))

	st1 := makeColorPtr(13, 14, 15, 16)
	s.SetStyle(makeStyle(nil, st1))

	if s.style.Fill != f0 {
		t.Fail()
	}
	if s.style.Stroke != st1 {
		t.Fail()
	}
}

func TestSetStyle_NilStrokeKeepsOld(t *testing.T) {
	s := NewShape(Rect, 1)

	f0 := makeColorPtr(1, 2, 3, 4)
	st0 := makeColorPtr(5, 6, 7, 8)
	s.SetStyle(makeStyle(f0, st0))

	f1 := makeColorPtr(9, 10, 11, 12)
	s.SetStyle(makeStyle(f1, nil))

	if s.style.Fill != f1 {
		t.Fail()
	}
	if s.style.Stroke != st0 {
		t.Fail()
	}
}

func TestObservers_UpdateRectOnMove(t *testing.T) {
	s := NewShape(Rect, 1)

	p0 := graphics.Point{X: 1, Y: 2}
	b0 := graphics.Bounds{Width: 3, Height: 4}
	WithPosition(p0)(s)
	WithBounds(b0)(s)

	o1 := &mockObserver{}
	o2 := &mockObserver{}
	o3 := &mockObserver{}
	s.AddObserver(o1)
	s.AddObserver(o2)
	s.AddObserver(o3)

	d := graphics.Vector{X: 10, Y: -5}
	s.Move(d)

	wantPos := graphics.Point{X: 11, Y: -3}
	wantBounds := b0

	obs := []*mockObserver{o1, o2, o3}
	for _, o := range obs {
		if o.updateRectCount != 1 {
			t.Fail()
		}
		if o.updateStyleCount != 0 {
			t.Fail()
		}
		if o.lastPos != wantPos {
			t.Fail()
		}
		if o.lastBounds != wantBounds {
			t.Fail()
		}
	}
}

func TestObservers_UpdateStyleOnSetStyle(t *testing.T) {
	s := NewShape(Rect, 1)

	o1 := &mockObserver{}
	o2 := &mockObserver{}
	o3 := &mockObserver{}
	s.AddObserver(o1)
	s.AddObserver(o2)
	s.AddObserver(o3)

	f1 := makeColorPtr(9, 10, 11, 12)
	st1 := makeColorPtr(13, 14, 15, 16)
	arg := makeStyle(f1, st1)

	s.SetStyle(arg)

	obs := []*mockObserver{o1, o2, o3}
	for _, o := range obs {
		if o.updateRectCount != 0 {
			t.Fail()
		}
		if o.updateStyleCount != 1 {
			t.Fail()
		}
		if o.lastStyle.Fill != arg.Fill || o.lastStyle.Stroke != arg.Stroke {
			t.Fail()
		}
	}
}
