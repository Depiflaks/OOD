package history

import (
	"image/color"
	"testing"
	"vector-editor/graphics"
	"vector-editor/model"
)

type mockShape struct {
	id      model.ShapeId
	pos     graphics.Vector
	size    graphics.Bounds
	style   graphics.Style
	marked  bool
	deleted bool
}

func newMockShape(id model.ShapeId) *mockShape {
	return &mockShape{

		id:    id,
		pos:   graphics.Vector{X: 0, Y: 0},
		size:  graphics.Bounds{Width: 10, Height: 10},
		style: graphics.Style{},
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
		func(d graphics.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		graphics.Vector{X: 5, Y: 5},
	)
	cmd.Unexecute()
	if shape.pos.X != 0 || shape.pos.Y != 0 {
		t.Fail()
	}
}

func TestMove_Execute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewMoveShapesCommand(
		func(d graphics.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		graphics.Vector{X: 5, Y: 5},
	)
	cmd.Execute()
	if shape.pos.X != 5 || shape.pos.Y != 5 {
		t.Fail()
	}
}

func TestMove_ExecuteUnexecute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewMoveShapesCommand(
		func(d graphics.Vector) {
			shape.pos.X += d.X
			shape.pos.Y += d.Y
		},
		graphics.Vector{X: 5, Y: 5},
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
		func(d graphics.Vector, b graphics.Bounds) {
			shape.size = b
		},
		graphics.Vector{},
		graphics.Bounds{Width: 20, Height: 20},
	)
	cmd.Unexecute()
	if shape.size.Width != 10 || shape.size.Height != 10 {
		t.Fail()
	}
}

func TestResize_Execute(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewResizeShapesCommand(
		func(d graphics.Vector, b graphics.Bounds) {
			shape.size = b
		},
		graphics.Vector{},
		graphics.Bounds{Width: 20, Height: 20},
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
//		func(d graphics.Vector, b graphics.Bounds) {
//			shape.size = b
//		},
//		graphics.Vector{},
//		graphics.Bounds{Width: 20, Height: 20},
//	)
//	cmd.Execute()
//	cmd.Unexecute()
//	if shape.size.Width != 10 || shape.size.Height != 10 {
//		t.Fail()
//	}
//}

func TestSetStyle_NotExecuted(t *testing.T) {
	shape := newMockShape(1)
	cmd := NewSetStyleCommand(
		func(s graphics.Style) { shape.style = s },
		graphics.Style{},
		graphics.Style{Fill: color.RGBA{R: 1}},
	)
	cmd.Unexecute()
	if shape.style != (graphics.Style{}) {
		t.Fail()
	}
}

func TestSetStyle_Execute(t *testing.T) {
	shape := newMockShape(1)
	newStyle := graphics.Style{Fill: color.RGBA{R: 1}}
	cmd := NewSetStyleCommand(
		func(s graphics.Style) { shape.style = s },
		graphics.Style{},
		newStyle,
	)
	cmd.Execute()
	if shape.style != newStyle {
		t.Fail()
	}
}

func TestSetStyle_ExecuteUnexecute(t *testing.T) {
	shape := newMockShape(1)
	prev := graphics.Style{}
	newStyle := graphics.Style{Fill: color.RGBA{R: 1}}
	cmd := NewSetStyleCommand(
		func(s graphics.Style) { shape.style = s },
		prev,
		newStyle,
	)
	cmd.Execute()
	cmd.Unexecute()
	if shape.style != prev {
		t.Fail()
	}
}

func TestSetStyle_ExecuteDispose(t *testing.T) {
	shape := newMockShape(1)
	newStyle := graphics.Style{Fill: color.RGBA{R: 1}}
	cmd := NewSetStyleCommand(
		func(s graphics.Style) { shape.style = s },
		graphics.Style{},
		newStyle,
	)
	cmd.Execute()
	if shape.style != newStyle {
		t.Fail()
	}
}
