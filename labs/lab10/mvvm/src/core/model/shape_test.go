package model

import (
	"image/color"
	"testing"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
	"vector-editor/src/types/geometry"
)

type mockObserver struct {
	updateRectCount  int
	updateStyleCount int

	lastPos    geometry.Point
	lastBounds geometry.Bounds
	lastStyle  draw.Style
}

func (m *mockObserver) UpdateRect(position geometry.Point, bounds geometry.Bounds) {
	m.updateRectCount++
	m.lastPos = position
	m.lastBounds = bounds
}

func (m *mockObserver) UpdateStyle(style draw.Style) {
	m.updateStyleCount++
	m.lastStyle = style
}

func makeColorPtr(r, g, b, a uint8) color.Color {
	var c color.Color = color.RGBA{R: r, G: g, B: b, A: a}
	return c
}

func makeStyle(fill, stroke color.Color) draw.Style {
	return draw.Style{Fill: fill, Stroke: stroke}
}

func newTestShape(t types.ShapeType, id int) *shape {
	sInt := NewShape(t, types.ShapeId(id), draw.Style{})
	return sInt.(*shape)
}

func TestWithPosition_AllFields(t *testing.T) {
	s := newTestShape(types.Rect, 7)

	p := geometry.Point{X: 10, Y: 20}
	WithPosition(p)(s)

	if s.id != 7 {
		t.Fail()
	}
	if s.shapeType != types.Rect {
		t.Fail()
	}
	if s.position != p {
		t.Fail()
	}
	// Исправлено: ожидаем дефолтный размер из model.go, а не пустой
	defaultSize := geometry.Bounds{Width: 60, Height: 120}
	if s.size != defaultSize {
		t.Errorf("Expected size %v, got %v", defaultSize, s.size)
	}
	// Стиль пустой, но поля в нем nil. Сравниваем с пустым значением структуры
	if s.style.Fill != nil || s.style.Stroke != nil {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestWithBounds_AllFields(t *testing.T) {
	s := newTestShape(types.Ellipse, 9)

	b := geometry.Bounds{Width: 30, Height: 40}
	WithBounds(b)(s)

	if s.id != 9 {
		t.Fail()
	}
	if s.shapeType != types.Ellipse {
		t.Fail()
	}
	// Исправлено: ожидаем дефолтную позицию из model.go
	defaultPos := geometry.Point{X: 100, Y: 100}
	if s.position != defaultPos {
		t.Errorf("Expected pos %v, got %v", defaultPos, s.position)
	}
	if s.size != b {
		t.Fail()
	}
	if s.style.Fill != nil {
		t.Fail()
	}
	if len(s.observers) != 0 {
		t.Fail()
	}
}

func TestMove_AllFields(t *testing.T) {
	s := newTestShape(types.Triangle, 11)

	p0 := geometry.Point{X: 1, Y: 2}
	b0 := geometry.Bounds{Width: 3, Height: 4}
	f0 := makeColorPtr(1, 2, 3, 4)
	st0 := makeColorPtr(5, 6, 7, 8)

	WithPosition(p0)(s)
	WithBounds(b0)(s)
	// Используем метод интерфейса или структуры, тут не важно
	s.SetStyle(makeStyle(f0, st0))

	d := geometry.Vector{X: 10, Y: -5}
	s.Move(d)

	if s.id != 11 {
		t.Fail()
	}
	if s.shapeType != types.Triangle {
		t.Fail()
	}
	if s.position != (geometry.Point{X: 11, Y: -3}) {
		t.Errorf("Expected pos {11, -3}, got %v", s.position)
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

func TestUpdateRect_AllFields(t *testing.T) {
	s := newTestShape(types.Ellipse, 13)

	p0 := geometry.Point{X: 1, Y: 1}
	b0 := geometry.Bounds{Width: 2, Height: 2}
	f0 := makeColorPtr(1, 1, 1, 255)
	st0 := makeColorPtr(2, 2, 2, 255)

	WithPosition(p0)(s)
	WithBounds(b0)(s)
	s.SetStyle(makeStyle(f0, st0))

	d := geometry.Point{X: 3, Y: 4}
	b1 := geometry.Bounds{Width: 10, Height: 20}
	s.UpdateRect(d, b1)

	if s.id != 13 {
		t.Fail()
	}
	if s.shapeType != types.Ellipse {
		t.Fail()
	}
	if s.position != (geometry.Point{X: 3, Y: 4}) {
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
	s := newTestShape(types.Rect, 1)

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
	s := newTestShape(types.Rect, 1)

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
	s := newTestShape(types.Rect, 1)

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
	s := newTestShape(types.Rect, 1)

	p0 := geometry.Point{X: 1, Y: 2}
	b0 := geometry.Bounds{Width: 3, Height: 4}
	WithPosition(p0)(s)
	WithBounds(b0)(s)

	o1 := &mockObserver{}
	o2 := &mockObserver{}
	o3 := &mockObserver{}
	s.AddObserver(o1)
	s.AddObserver(o2)
	s.AddObserver(o3)

	d := geometry.Vector{X: 10, Y: -5}
	s.Move(d)

	wantPos := geometry.Point{X: 11, Y: -3}
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
	s := newTestShape(types.Rect, 1)

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
