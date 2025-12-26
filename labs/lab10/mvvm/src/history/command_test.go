package history

import (
	"image/color"
	"math"
	"testing"
	"vector-editor/src/geometry"
	"vector-editor/src/model"
)

type mockShape struct {
	id      model.ShapeId
	pos     geometry.Vector
	size    geometry.Bounds
	style   geometry.Style
	marked  bool
	deleted bool
}

func newMockShape(id model.ShapeId) *mockShape {
	return &mockShape{

		id:    id,
		pos:   geometry.Vector{X: 0, Y: 0},
		size:  geometry.Bounds{Width: 10, Height: 10},
		style: geometry.Style{},
	}
}

func TestNewShape_NotExecuted(t *testing.T) {
	var created, marked, deleted int
	cmd := NewNewShapeCommand(
		func() model.ShapeId { created++; return 1 },
		func(ids []model.ShapeId) { marked++ },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Unexecute()
	cmd.Dispose()
	if created != 0 || marked != 0 || deleted != 0 {
		t.Fail()
	}
}

func TestNewShape_Execute(t *testing.T) {
	var created int
	cmd := NewNewShapeCommand(
		func() model.ShapeId { created++; return 1 },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
	)
	cmd.Execute()
	if created != 1 {
		t.Fail()
	}
}

func TestNewShape_ExecuteUnexecute(t *testing.T) {
	var marked int
	cmd := NewNewShapeCommand(
		func() model.ShapeId { return 1 },
		func(ids []model.ShapeId) { marked++ },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
	)
	cmd.Execute()
	cmd.Unexecute()
	if marked != 1 {
		t.Fail()
	}
}

func TestNewShape_ExecuteUnexecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewNewShapeCommand(
		func() model.ShapeId { return 1 },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Unexecute()
	cmd.Dispose()
	if deleted != 1 {
		t.Fail()
	}
}

func TestNewShape_ExecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewNewShapeCommand(
		func() model.ShapeId { return 1 },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Dispose()
	if deleted != 1 {
		t.Fail()
	}
}

func TestDeleteShape_NotExecuted(t *testing.T) {
	var restored, deleted int
	cmd := NewDeleteShapeCommand(
		[]model.ShapeId{1},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { restored++ },
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Unexecute()
	cmd.Dispose()
	if restored != 0 || deleted != 0 {
		t.Fail()
	}
}

func TestDeleteShape_Execute(t *testing.T) {
	var marked int
	cmd := NewDeleteShapeCommand(
		[]model.ShapeId{1},
		func(ids []model.ShapeId) { marked++ },
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
	)
	cmd.Execute()
	if marked != 1 {
		t.Fail()
	}
}

func TestDeleteShape_ExecuteUnexecute(t *testing.T) {
	var restored int
	cmd := NewDeleteShapeCommand(
		[]model.ShapeId{1},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { restored++ },
		func(ids []model.ShapeId) {},
	)
	cmd.Execute()
	cmd.Unexecute()
	if restored != 1 {
		t.Fail()
	}
}

func TestDeleteShape_ExecuteUnexecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewDeleteShapeCommand(
		[]model.ShapeId{1},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Unexecute()
	cmd.Dispose()
	if deleted != 1 {
		t.Fail()
	}
}

func TestDeleteShape_ExecuteDispose(t *testing.T) {
	var deleted int
	cmd := NewDeleteShapeCommand(
		[]model.ShapeId{1},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) {},
		func(ids []model.ShapeId) { deleted++ },
	)
	cmd.Execute()
	cmd.Dispose()
	if deleted != 1 {
		t.Fail()
	}
}

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
		t.Fail()
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
		t.Fail()
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
		t.Fail()
	}
}

type mockShapeRect struct {
	id   model.ShapeId
	pos  geometry.Point
	size geometry.Bounds
}

func newMockShapeRect(id model.ShapeId) *mockShapeRect {
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
	return feq(a.X, b.X) && feq(a.Y, b.Y)
}

func boundsEq(a, b geometry.Bounds) bool {
	return feq(a.Width, b.Width) && feq(a.Height, b.Height)
}

func rectEqPosSize(s *mockShapeRect, pos geometry.Point, size geometry.Bounds) bool {
	return pointEq(s.pos, pos) && boundsEq(s.size, size)
}

func TestResize_NotExecuted(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	curRects := map[model.ShapeId]geometry.Rect{
		1: {Position: shape.pos, Size: shape.size},
	}
	newRects := map[model.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 5, Y: 7}, Size: geometry.Bounds{Width: 20, Height: 20}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Unexecute()

	if !rectEqPosSize(shape, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Fail()
	}
}

func TestResize_Execute(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	curRects := map[model.ShapeId]geometry.Rect{
		1: {Position: shape.pos, Size: shape.size},
	}
	wantPos := geometry.Point{X: 5, Y: 7}
	wantBounds := geometry.Bounds{Width: 20, Height: 20}
	newRects := map[model.ShapeId]geometry.Rect{
		1: {Position: wantPos, Size: wantBounds},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()

	if !rectEqPosSize(shape, wantPos, wantBounds) {
		t.Fail()
	}
}

func TestResize_ExecuteUnexecute(t *testing.T) {
	shape := newMockShapeRect(1)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r := rects[shape.id]
		shape.pos = r.Position
		shape.size = r.Size
	}

	startPos := shape.pos
	startBounds := shape.size

	curRects := map[model.ShapeId]geometry.Rect{
		1: {Position: startPos, Size: startBounds},
	}
	newRects := map[model.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 5, Y: 7}, Size: geometry.Bounds{Width: 20, Height: 20}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()
	cmd.Unexecute()

	if !rectEqPosSize(shape, startPos, startBounds) {
		t.Fail()
	}
}

func TestResize_Multi_NotExecuted(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r1 := rects[s1.id]
		r2 := rects[s2.id]
		r3 := rects[s3.id]
		s1.pos, s1.size = r1.Position, r1.Size
		s2.pos, s2.size = r2.Position, r2.Size
		s3.pos, s3.size = r3.Position, r3.Size
	}

	curRects := map[model.ShapeId]geometry.Rect{
		1: {Position: s1.pos, Size: s1.size},
		2: {Position: s2.pos, Size: s2.size},
		3: {Position: s3.pos, Size: s3.size},
	}
	newRects := map[model.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}},
		2: {Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}},
		3: {Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Unexecute()

	if !rectEqPosSize(s1, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Fail()
	}
	if !rectEqPosSize(s2, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Fail()
	}
	if !rectEqPosSize(s3, geometry.Point{X: 0, Y: 0}, geometry.Bounds{Width: 10, Height: 10}) {
		t.Fail()
	}
}

func TestResize_Multi_Execute(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r1 := rects[s1.id]
		r2 := rects[s2.id]
		r3 := rects[s3.id]
		s1.pos, s1.size = r1.Position, r1.Size
		s2.pos, s2.size = r2.Position, r2.Size
		s3.pos, s3.size = r3.Position, r3.Size
	}

	curRects := map[model.ShapeId]geometry.Rect{
		1: {Position: s1.pos, Size: s1.size},
		2: {Position: s2.pos, Size: s2.size},
		3: {Position: s3.pos, Size: s3.size},
	}

	want1 := geometry.Rect{Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}}
	want2 := geometry.Rect{Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}}
	want3 := geometry.Rect{Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}}

	newRects := map[model.ShapeId]geometry.Rect{
		1: want1,
		2: want2,
		3: want3,
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()

	if !rectEqPosSize(s1, want1.Position, want1.Size) {
		t.Fail()
	}
	if !rectEqPosSize(s2, want2.Position, want2.Size) {
		t.Fail()
	}
	if !rectEqPosSize(s3, want3.Position, want3.Size) {
		t.Fail()
	}
}

func TestResize_Multi_ExecuteUnexecute(t *testing.T) {
	s1 := newMockShapeRect(1)
	s2 := newMockShapeRect(2)
	s3 := newMockShapeRect(3)

	resize := func(rects map[model.ShapeId]geometry.Rect) {
		r1 := rects[s1.id]
		r2 := rects[s2.id]
		r3 := rects[s3.id]
		s1.pos, s1.size = r1.Position, r1.Size
		s2.pos, s2.size = r2.Position, r2.Size
		s3.pos, s3.size = r3.Position, r3.Size
	}

	start1 := geometry.Rect{Position: s1.pos, Size: s1.size}
	start2 := geometry.Rect{Position: s2.pos, Size: s2.size}
	start3 := geometry.Rect{Position: s3.pos, Size: s3.size}

	curRects := map[model.ShapeId]geometry.Rect{
		1: start1,
		2: start2,
		3: start3,
	}
	newRects := map[model.ShapeId]geometry.Rect{
		1: {Position: geometry.Point{X: 1, Y: 2}, Size: geometry.Bounds{Width: 20, Height: 20}},
		2: {Position: geometry.Point{X: 3, Y: 4}, Size: geometry.Bounds{Width: 30, Height: 40}},
		3: {Position: geometry.Point{X: 5, Y: 6}, Size: geometry.Bounds{Width: 50, Height: 60}},
	}

	cmd := NewResizeShapesCommand(resize, newRects, curRects)
	cmd.Execute()
	cmd.Unexecute()

	if !rectEqPosSize(s1, start1.Position, start1.Size) {
		t.Fail()
	}
	if !rectEqPosSize(s2, start2.Position, start2.Size) {
		t.Fail()
	}
	if !rectEqPosSize(s3, start3.Position, start3.Size) {
		t.Fail()
	}
}

type mockShapeStyle struct {
	id    model.ShapeId
	style geometry.Style
}

func newMockShapeStyle(id model.ShapeId) *mockShapeStyle {
	return &mockShapeStyle{id: id}
}

func colorPtr(r, g, b, a uint8) *color.Color {
	var c color.Color = color.RGBA{R: r, G: g, B: b, A: a}
	return &c
}

func stylesEqual(a, b geometry.Style) bool {
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

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Unexecute()

	if !stylesEqual(shape.style, geometry.Style{}) {
		t.Fail()
	}
}

func TestSetStyle_Execute(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {},
	}
	newStyle := geometry.Style{Fill: colorPtr(1, 0, 0, 255)}
	newStyles := map[model.ShapeId]geometry.Style{
		1: newStyle,
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(shape.style, newStyle) {
		t.Fail()
	}
}

func TestSetStyle_ExecuteUnexecute(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		shape.style = styles[shape.id]
	}

	prev := geometry.Style{Fill: colorPtr(9, 0, 0, 255)}
	prevStyles := map[model.ShapeId]geometry.Style{
		1: prev,
	}
	newStyle := geometry.Style{Fill: colorPtr(1, 0, 0, 255)}
	newStyles := map[model.ShapeId]geometry.Style{
		1: newStyle,
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(shape.style, prev) {
		t.Fail()
	}
}

func TestSetStyle_ExecuteDispose(t *testing.T) {
	shape := newMockShapeStyle(1)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		shape.style = styles[shape.id]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {},
	}
	newStyle := geometry.Style{Fill: colorPtr(1, 0, 0, 255)}
	newStyles := map[model.ShapeId]geometry.Style{
		1: newStyle,
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(shape.style, newStyle) {
		t.Fail()
	}
}

func TestSetStyle_Multi_NotExecuted_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		s1.style = styles[1]
		s2.style = styles[2]
		s3.style = styles[3]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(10, 0, 0, 255)},
		2: {Fill: colorPtr(20, 0, 0, 255)},
		3: {Fill: colorPtr(30, 0, 0, 255)},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		2: {Fill: colorPtr(2, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Unexecute()

	if !stylesEqual(s1.style, geometry.Style{}) || !stylesEqual(s2.style, geometry.Style{}) || !stylesEqual(s3.style, geometry.Style{}) {
		t.Fail()
	}
}

func TestSetStyle_Multi_Execute_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		s1.style = styles[1]
		s2.style = styles[2]
		s3.style = styles[3]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {},
		2: {},
		3: {},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		2: {Fill: colorPtr(2, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) || !stylesEqual(s2.style, newStyles[2]) || !stylesEqual(s3.style, newStyles[3]) {
		t.Fail()
	}
}

func TestSetStyle_Multi_ExecuteUnexecute_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		s1.style = styles[1]
		s2.style = styles[2]
		s3.style = styles[3]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(10, 0, 0, 255)},
		2: {Fill: colorPtr(20, 0, 0, 255)},
		3: {Fill: colorPtr(30, 0, 0, 255)},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		2: {Fill: colorPtr(2, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Fail()
	}
}

func TestSetStyle_Multi_ExecuteUnexecuteDispose_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		s1.style = styles[1]
		s2.style = styles[2]
		s3.style = styles[3]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(10, 0, 0, 255)},
		2: {Fill: colorPtr(20, 0, 0, 255)},
		3: {Fill: colorPtr(30, 0, 0, 255)},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		2: {Fill: colorPtr(2, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Fail()
	}
}

func TestSetStyle_Multi_ExecuteDispose_Full(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
		s1.style = styles[1]
		s2.style = styles[2]
		s3.style = styles[3]
	}

	prevStyles := map[model.ShapeId]geometry.Style{
		1: {},
		2: {},
		3: {},
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		2: {Fill: colorPtr(2, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) || !stylesEqual(s2.style, newStyles[2]) || !stylesEqual(s3.style, newStyles[3]) {
		t.Fail()
	}
}

func TestSetStyle_Multi_Execute_Partial(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	s1.style = geometry.Style{Fill: colorPtr(10, 0, 0, 255)}
	s2.style = geometry.Style{Fill: colorPtr(20, 0, 0, 255)}
	s3.style = geometry.Style{Fill: colorPtr(30, 0, 0, 255)}

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
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

	prevStyles := map[model.ShapeId]geometry.Style{
		1: s1.style,
		2: s2.style,
		3: s3.style,
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()

	if !stylesEqual(s1.style, newStyles[1]) {
		t.Fail()
	}
	if !stylesEqual(s2.style, prevStyles[2]) {
		t.Fail()
	}
	if !stylesEqual(s3.style, newStyles[3]) {
		t.Fail()
	}
}

func TestSetStyle_Multi_ExecuteUnexecute_Partial(t *testing.T) {
	s1 := newMockShapeStyle(1)
	s2 := newMockShapeStyle(2)
	s3 := newMockShapeStyle(3)

	s1.style = geometry.Style{Fill: colorPtr(10, 0, 0, 255)}
	s2.style = geometry.Style{Fill: colorPtr(20, 0, 0, 255)}
	s3.style = geometry.Style{Fill: colorPtr(30, 0, 0, 255)}

	setStyle := func(styles map[model.ShapeId]geometry.Style) {
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

	prevStyles := map[model.ShapeId]geometry.Style{
		1: s1.style,
		2: s2.style,
		3: s3.style,
	}
	newStyles := map[model.ShapeId]geometry.Style{
		1: {Fill: colorPtr(1, 0, 0, 255)},
		3: {Fill: colorPtr(3, 0, 0, 255)},
	}

	cmd := NewSetStyleCommand(setStyle, prevStyles, newStyles)
	cmd.Execute()
	cmd.Unexecute()

	if !stylesEqual(s1.style, prevStyles[1]) || !stylesEqual(s2.style, prevStyles[2]) || !stylesEqual(s3.style, prevStyles[3]) {
		t.Fail()
	}
}
