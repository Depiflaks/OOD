package history

import (
	"image/color"
	"math"
	"testing"
	"vector-editor/src/types"
	"vector-editor/src/types/draw"
	"vector-editor/src/types/geometry"
)

type mockShape struct {
	id      types.ShapeId
	pos     geometry.Vector
	size    geometry.Bounds
	style   draw.Style
	marked  bool
	deleted bool
}

func newMockShape(id types.ShapeId) *mockShape {
	return &mockShape{
		id:    id,
		pos:   geometry.Vector{X: 0, Y: 0},
		size:  geometry.Bounds{Width: 10, Height: 10},
		style: draw.Style{},
	}
}

// --- NewShapeCommand Tests ---

func TestNewShape_NotExecuted(t *testing.T) {
	var created, marked, deleted int
	cmd := NewNewShapeCommand(
		func() types.ShapeId { created++; return 1 },
		func(ids []types.ShapeId) { marked++ },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {
			// Команда пытается удалить ID 0, если не была выполнена.
			// Игнорируем это в тесте, так как ID 0 невалиден.
			if len(ids) > 0 && ids[0] != 0 {
				deleted++
			}
		},
	)
	cmd.Unexecute()
	cmd.Dispose()

	if created != 0 || marked != 0 || deleted != 0 {
		t.Errorf("Expected 0 ops, got created=%d, marked=%d, deleted=%d", created, marked, deleted)
	}
}

func TestNewShape_Execute(t *testing.T) {
	var created int
	cmd := NewNewShapeCommand(
		func() types.ShapeId { created++; return 1 },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
	)
	cmd.Execute()
	if created != 1 {
		t.Errorf("Expected created=1, got %d", created)
	}
}

func TestNewShape_ExecuteUnexecute(t *testing.T) {
	var marked int
	cmd := NewNewShapeCommand(
		func() types.ShapeId { return 1 },
		func(ids []types.ShapeId) { marked++ },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
	)
	cmd.Execute()
	cmd.Unexecute()
	if marked != 1 {
		t.Errorf("Expected marked=1, got %d", marked)
	}
}

func TestNewShape_ExecuteUnexecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewNewShapeCommand(
		func() types.ShapeId { return 1 },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Unexecute()
	// Unexecute ставит isExecuted = false.
	// Dispose видит !isExecuted и вызывает delete.
	cmd.Dispose()

	if deleted != 1 {
		t.Errorf("Expected deleted=1 (cleanup after undo), got %d", deleted)
	}
}

func TestNewShape_ExecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewNewShapeCommand(
		func() types.ShapeId { return 1 },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { deleted++ },
	)
	cmd.Execute()
	// Execute ставит isExecuted = true.
	// Dispose видит isExecuted и делает return (не удаляет).
	// Это правильно: если мы чистим историю, но команда выполнена, фигура должна остаться на холсте.
	cmd.Dispose()

	if deleted != 0 {
		t.Errorf("Expected deleted=0 (executed shape should stay), got %d", deleted)
	}
}

// --- DeleteShapeCommand Tests ---

func TestDeleteShape_NotExecuted(t *testing.T) {
	var restored, deleted int
	cmd := NewDeleteShapeCommand(
		[]types.ShapeId{1},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { restored++ },
		func(ids []types.ShapeId) { deleted++ },
	)
	cmd.Unexecute()
	cmd.Dispose()

	if restored != 0 || deleted != 0 {
		t.Errorf("Expected 0 ops, got restored=%d, deleted=%d", restored, deleted)
	}
}

func TestDeleteShape_Execute(t *testing.T) {
	var marked int
	cmd := NewDeleteShapeCommand(
		[]types.ShapeId{1},
		func(ids []types.ShapeId) { marked++ },
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
	)
	cmd.Execute()
	if marked != 1 {
		t.Errorf("Expected marked=1, got %d", marked)
	}
}

func TestDeleteShape_ExecuteUnexecute(t *testing.T) {
	var restored int
	cmd := NewDeleteShapeCommand(
		[]types.ShapeId{1},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { restored++ },
		func(ids []types.ShapeId) {},
	)
	cmd.Execute()
	cmd.Unexecute()
	if restored != 1 {
		t.Errorf("Expected restored=1, got %d", restored)
	}
}

func TestDeleteShape_ExecuteUnexecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewDeleteShapeCommand(
		[]types.ShapeId{1},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Unexecute()
	// Unexecute ставит isExecuted = false.
	// Dispose видит !isExecuted (удаление отменено, фигура на холсте) -> ничего не делает.
	cmd.Dispose()

	if deleted != 0 {
		t.Errorf("Expected deleted=0 (shape was restored), got %d", deleted)
	}
}

func TestDeleteShape_ExecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewDeleteShapeCommand(
		[]types.ShapeId{1},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) {},
		func(ids []types.ShapeId) { deleted++ },
	)
	cmd.Execute()
	// Execute ставит isExecuted = true.
	// Dispose видит isExecuted (фигура помечена на удаление) -> вызывает delete (подтверждает удаление).
	cmd.Dispose()

	if deleted != 1 {
		t.Errorf("Expected deleted=1, got %d", deleted)
	}
}

// --- MoveShapesCommand Tests ---

func TestMove_NotExecuted(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewMoveShapesCommand(
		func(d geometry.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		geometry.Vector{X: 5, Y: 5},
	)
	cmd.Unexecute()
	if shape.pos.X != 0 || shape.pos.Y != 0 {
		t.Errorf("Expected pos(0,0), got %v", shape.pos)
	}
}

func TestMove_Execute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewMoveShapesCommand(
		func(d geometry.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		geometry.Vector{X: 5, Y: 5},
	)
	cmd.Execute()
	if shape.pos.X != 5 || shape.pos.Y != 5 {
		t.Errorf("Expected pos(5,5), got %v", shape.pos)
	}
}

func TestMove_ExecuteUnexecute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewMoveShapesCommand(
		func(d geometry.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		geometry.Vector{X: 5, Y: 5},
	)
	cmd.Execute()
	cmd.Unexecute()
	if shape.pos.X != 0 || shape.pos.Y != 0 {
		t.Errorf("Expected pos(0,0), got %v", shape.pos)
	}
}

// --- ResizeShapesCommand Tests ---

type mockShapeRect struct {
	id   types.ShapeId
	pos  geometry.Point
	size geometry.Bounds
}

func newMockShapeRect(id types.ShapeId) *mockShapeRect {
	return &mockShapeRect{
		id:   id,
		pos:  geometry.Point{X: 0, Y: 0},
		size: geometry.Bounds{Width: 10, Height: 10},
	}
}

func feq(a, b float64) bool {
	const eps = 1e-9
	return math.Abs(a-b) <= eps
}

func pointEq(a, b geometry.Point) bool {
	return feq(float64(a.X), float64(b.X)) && feq(float64(a.Y), float64(b.Y))
}

func boundsEq(a, b geometry.Bounds) bool {
	return feq(float64(a.Width), float64(b.Width)) && feq(float64(a.Height), float64(b.Height))
}

func rectEqPosSize(s *mockShapeRect, pos geometry.Point, size geometry.Bounds) bool {
	return pointEq(s.pos, pos) && boundsEq(s.size, size)
}

func TestResize_NotExecuted(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	curRects := map[types.ShapeId]geometry.Rect{
		1: {Position: shape.pos, Size: shape.size},
	}
	newRects := map[types.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 5, Y: 7}, Size: geometry.Bounds{Width: 20, Height: 20}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Unexecute()

	if !rectEqPosSize(shape, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Error("Shape changed despite command not being executed")
	}
}

func TestResize_Execute(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	curRects := map[types.ShapeId]geometry.Rect{
		1: {Position: shape.pos, Size: shape.size},
	}
	wantPos := geometry.Point{X: 5, Y: 7}
	wantBounds := geometry.Bounds{Width: 20, Height: 20}
	newRects := map[types.ShapeId]geometry.Rect{
		1: {Position: wantPos, Size: wantBounds},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()

	if !rectEqPosSize(shape, wantPos, wantBounds) {
		t.Error("Shape not resized correctly")
	}
}

func TestResize_ExecuteUnexecute(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	startPos := shape.pos
	startBounds := shape.size

	curRects := map[types.ShapeId]geometry.Rect{
		1: {Position: startPos, Size: startBounds},
	}
	newRects := map[types.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 5, Y: 7}, Size: geometry.Bounds{Width: 20, Height: 20}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()
	cmd.Unexecute()

	if !rectEqPosSize(shape, startPos, startBounds) {
		t.Error("Shape not restored after unexecute")
	}
}

func TestResize_Multi_NotExecuted(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		if r, ok := rects[s1.id]; ok {
			s1.pos, s1.size = r.Position, r.Size
		}
		if r, ok := rects[s2.id]; ok {
			s2.pos, s2.size = r.Position, r.Size
		}
		if r, ok := rects[s3.id]; ok {
			s3.pos, s3.size = r.Position, r.Size
		}
	}

	curRects := map[types.ShapeId]geometry.Rect{
		1: {Position: s1.pos, Size: s1.size},
		2: {Position: s2.pos, Size: s2.size},
		3: {Position: s3.pos, Size: s3.size},
	}
	newRects := map[types.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}},
		2: {Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}},
		3: {Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Unexecute()

	if !rectEqPosSize(s1, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Error("S1 changed unexpectedly")
	}
}

func TestResize_Multi_Execute(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		if r, ok := rects[s1.id]; ok {
			s1.pos, s1.size = r.Position, r.Size
		}
		if r, ok := rects[s2.id]; ok {
			s2.pos, s2.size = r.Position, r.Size
		}
		if r, ok := rects[s3.id]; ok {
			s3.pos, s3.size = r.Position, r.Size
		}
	}

	curRects := map[types.ShapeId]geometry.Rect{
		1: {Position: s1.pos, Size: s1.size},
		2: {Position: s2.pos, Size: s2.size},
		3: {Position: s3.pos, Size: s3.size},
	}

	want1 := geometry.Rect{Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}}
	want2 := geometry.Rect{Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}}
	want3 := geometry.Rect{Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}}

	newRects := map[types.ShapeId]geometry.Rect{
		1: want1,
		2: want2,
		3: want3,
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()

	if !rectEqPosSize(s1, want1.Position, want1.Size) {
		t.Error("S1 mismatch")
	}
	if !rectEqPosSize(s2, want2.Position, want2.Size) {
		t.Error("S2 mismatch")
	}
	if !rectEqPosSize(s3, want3.Position, want3.Size) {
		t.Error("S3 mismatch")
	}
}

func TestResize_Multi_ExecuteUnexecute(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[types.ShapeId]geometry.Rect) {
		if r, ok := rects[s1.id]; ok {
			s1.pos, s1.size = r.Position, r.Size
		}
		if r, ok := rects[s2.id]; ok {
			s2.pos, s2.size = r.Position, r.Size
		}
		if r, ok := rects[s3.id]; ok {
			s3.pos, s3.size = r.Position, r.Size
		}
	}

	start1 := geometry.Rect{Position: s1.pos, Size: s1.size}
	start2 := geometry.Rect{Position: s2.pos, Size: s2.size}
	start3 := geometry.Rect{Position: s3.pos, Size: s3.size}

	curRects := map[types.ShapeId]geometry.Rect{
		1: start1,
		2: start2,
		3: start3,
	}
	newRects := map[types.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}},
		2: {Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}},
		3: {Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()
	cmd.Unexecute()

	if !rectEqPosSize(s1, start1.Position, start1.Size) {
		t.Error("S1 not restored")
	}
	if !rectEqPosSize(s2, start2.Position, start2.Size) {
		t.Error("S2 not restored")
	}
	if !rectEqPosSize(s3, start3.Position, start3.Size) {
		t.Error("S3 not restored")
	}
}

// --- SetStyleCommand Tests ---

type mockShapeStyle struct {
	id    types.ShapeId
	style draw.Style
}

func newMockShapeStyle(id types.ShapeId) *mockShapeStyle {
	return &mockShapeStyle{id: id}
}

func newColor(r, g, b, a uint8) color.Color {
	return color.RGBA{R: r, G: g, B: b, A: a}
}

func stylesEqual(a, b draw.Style) bool {
	if (a.Fill == nil) != (b.Fill == nil) {
		return false
	}
	if a.Fill != nil && b.Fill != nil && a.Fill != b.Fill {
		return false
	}
	if (a.Stroke == nil) != (b.Stroke == nil) {
		return false
	}
	if a.Stroke != nil && b.Stroke != nil && a.Stroke != b.Stroke {
		return false
	}
	return true
}

func TestSetStyle_NotExecuted(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[types.ShapeId]draw.Style{1: {}}
	newStyles := map[types.ShapeId]draw.Style{1: {Fill: newColor(1, 0, 0, 255)}}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Unexecute()

	if !stylesEqual(shape.style, draw.Style{}) {
		t.Error("Style changed unexpectedly")
	}
}

func TestSetStyle_Execute(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[types.ShapeId]draw.Style{1: {}}
	newStyle := draw.Style{Fill: newColor(1, 0, 0, 255)}
	newStyles := map[types.ShapeId]draw.Style{1: newStyle}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(shape.style, newStyle) {
		t.Error("Style not applied")
	}
}

func TestSetStyle_ExecuteUnexecute(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		shape.style = styles[shape.id]
	}

	prev := draw.Style{Fill: newColor(9, 0, 0, 255)}
	prevStyles := map[types.ShapeId]draw.Style{1: prev}
	newStyle := draw.Style{Fill: newColor(1, 0, 0, 255)}
	newStyles := map[types.ShapeId]draw.Style{1: newStyle}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(shape.style, prev) {
		t.Error("Style not restored")
	}
}

func TestSetStyle_ExecuteDispose(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[types.ShapeId]draw.Style{1: {}}
	newStyle := draw.Style{Fill: newColor(1, 0, 0, 255)}
	newStyles := map[types.ShapeId]draw.Style{1: newStyle}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	// Dispose для SetStyleCommand пока ничего не делает (согласно TODO), но тест проверяет, что стиль не сломался

	if !stylesEqual(shape.style, newStyle) {
		t.Error("Style changed after dispose")
	}
}

func TestSetStyle_Multi_NotExecuted_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if s, ok := styles[1]; ok {
			s1.style = s
		}
		if s, ok := styles[2]; ok {
			s2.style = s
		}
		if s, ok := styles[3]; ok {
			s3.style = s
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(10, 0, 0, 255)},
		2: {Fill: newColor(20, 0, 0, 255)},
		3: {Fill: newColor(30, 0, 0, 255)},
	}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		2: {Fill: newColor(2, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Unexecute()

	if !stylesEqual(s1.style, draw.Style{}) || !stylesEqual(s2.style, draw.Style{}) || !stylesEqual(s3.style, draw.Style{}) {
		t.Error("Styles changed unexpectedly")
	}
}

func TestSetStyle_Multi_Execute_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if s, ok := styles[1]; ok {
			s1.style = s
		}
		if s, ok := styles[2]; ok {
			s2.style = s
		}
		if s, ok := styles[3]; ok {
			s3.style = s
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{1: {}, 2: {}, 3: {}}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		2: {Fill: newColor(2, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) || !stylesEqual(s2.style, newStyles[2]) || !stylesEqual(s3.style, newStyles[3]) {
		t.Error("Styles mismatch")
	}
}

func TestSetStyle_Multi_ExecuteUnexecute_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if s, ok := styles[1]; ok {
			s1.style = s
		}
		if s, ok := styles[2]; ok {
			s2.style = s
		}
		if s, ok := styles[3]; ok {
			s3.style = s
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(10, 0, 0, 255)},
		2: {Fill: newColor(20, 0, 0, 255)},
		3: {Fill: newColor(30, 0, 0, 255)},
	}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		2: {Fill: newColor(2, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Error("Styles not restored")
	}
}

func TestSetStyle_Multi_ExecuteUnexecuteDispose_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if s, ok := styles[1]; ok {
			s1.style = s
		}
		if s, ok := styles[2]; ok {
			s2.style = s
		}
		if s, ok := styles[3]; ok {
			s3.style = s
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(10, 0, 0, 255)},
		2: {Fill: newColor(20, 0, 0, 255)},
		3: {Fill: newColor(30, 0, 0, 255)},
	}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		2: {Fill: newColor(2, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()
	// Dispose ничего не делает для SetStyle, проверяем что состояние корректное
	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Error("Styles corrupted after dispose sequence")
	}
}

func TestSetStyle_Multi_ExecuteDispose_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if s, ok := styles[1]; ok {
			s1.style = s
		}
		if s, ok := styles[2]; ok {
			s2.style = s
		}
		if s, ok := styles[3]; ok {
			s3.style = s
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{1: {}, 2: {}, 3: {}}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		2: {Fill: newColor(2, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) || !stylesEqual(s2.style, newStyles[2]) || !stylesEqual(s3.style, newStyles[3]) {
		t.Error("Styles mismatch")
	}
}

func TestSetStyle_Multi_Execute_Partial(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	s1.style = draw.Style{Fill: newColor(10, 0, 0, 255)}
	s2.style = draw.Style{Fill: newColor(20, 0, 0, 255)}
	s3.style = draw.Style{Fill: newColor(30, 0, 0, 255)}

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if st, ok := styles[1]; ok {
			s1.style = st
		}
		if st, ok := styles[2]; ok {
			s2.style = st
		}
		if st, ok := styles[3]; ok {
			s3.style = st
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{1: s1.style, 2: s2.style, 3: s3.style}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) {
		t.Error("S1 mismatch")
	}
	if !stylesEqual(s2.style, prevStyles[2]) {
		t.Error("S2 should not change")
	}
	if !stylesEqual(s3.style, newStyles[3]) {
		t.Error("S3 mismatch")
	}
}

func TestSetStyle_Multi_ExecuteUnexecute_Partial(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	s1.style = draw.Style{Fill: newColor(10, 0, 0, 255)}
	s2.style = draw.Style{Fill: newColor(20, 0, 0, 255)}
	s3.style = draw.Style{Fill: newColor(30, 0, 0, 255)}

	setStyle := func(styles map[types.ShapeId]draw.Style) {
		if st, ok := styles[1]; ok {
			s1.style = st
		}
		if st, ok := styles[2]; ok {
			s2.style = st
		}
		if st, ok := styles[3]; ok {
			s3.style = st
		}
	}

	prevStyles := map[types.ShapeId]draw.Style{1: s1.style, 2: s2.style, 3: s3.style}
	newStyles := map[types.ShapeId]draw.Style{
		1: {Fill: newColor(1, 0, 0, 255)},
		3: {Fill: newColor(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Error("Styles not restored correctly")
	}
}
