package history

import (
	"image/color"
	"testing"
	"vector-editor/geometry"
	"vector-editor/model"
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

func TestResize_NotExecuted(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewResizeShapesCommand(
		func(d geometry.Vector, b geometry.Bounds) {
			shape.size = b
		},
		geometry.Vector{},
		geometry.Bounds{Width: 20, Height: 20},
	)
	cmd.Unexecute()
	if shape.size.Width != 10 || shape.size.Height != 10 {
		t.Fail()
	}
}

func TestResize_Execute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewResizeShapesCommand(
		func(d geometry.Vector, b geometry.Bounds) {
			shape.size = b
		},
		geometry.Vector{},
		geometry.Bounds{Width: 20, Height: 20},
	)
	cmd.Execute()
	if shape.size.Width != 20 || shape.size.Height != 20 {
		t.Fail()
	}
}

// TODO: исрпавить, когда будет готов мультиселект
//func TestResize_ExecuteUnexecute(t *testing.T) {
//	shape := newMockShape(1)
//	cmd := NewResizeShapesCommand(
//		func(d geometry.Vector, b geometry.Bounds) {
//			shape.size = b
//		},
//		geometry.Vector{},
//		geometry.Bounds{Width: 20, Height: 20},
//	)
//	cmd.Execute()
//	cmd.Unexecute()
//	if shape.size.Width != 10 || shape.size.Height != 10 {
//		t.Fail()
//	}
//}

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
