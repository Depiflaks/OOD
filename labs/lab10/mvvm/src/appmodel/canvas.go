package appmodel

import (
	history2 "vector-editor/src/history"
	"vector-editor/src/model"
)

type CanvasManager struct {
	history      history2.History
	canvas       EditableCanvas
	shapeManager *ShapeManager
}

func NewCanvasManager(
	h history2.History,
	canvas EditableCanvas,
) *CanvasManager {
	return &CanvasManager{
		history:      h,
		canvas:       canvas,
		shapeManager: NewShapeManager(&h),
	}
}

func (m *CanvasManager) NewShape(t model.ShapeType) {
	cmd := history2.NewNewShapeCommand(
		m.newCreateShapeFn(t),
		m.newMarkDeleteShapesFn(),
		m.newRestoreShapesFn(),
		m.newDeleteShapesFn(),
	)
	m.history.AppendAndExecute(cmd)
}

func (m *CanvasManager) ClearSelection() {
	m.shapeManager.ClearSelection()
}

func (m *CanvasManager) Delete() {
	ids := m.shapeManager.GetSelectedShapeIds()
	if len(ids) == 0 {
		return
	}
	cmd := history2.NewDeleteShapeCommand(
		ids,
		m.newMarkDeleteShapesFn(),
		m.newRestoreShapesFn(),
		m.newDeleteShapesFn(),
	)
	m.history.AppendAndExecute(cmd)
}

func (m *CanvasManager) newCreateShapeFn(t model.ShapeType) history2.CreateShapeFn {
	return func() model.ShapeId {
		return m.canvas.GetCanvas().NewShape(t)
	}
}

func (m *CanvasManager) newMarkDeleteShapesFn() history2.MarkDeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.MarkDeleted(ids)
	}
}

func (m *CanvasManager) newRestoreShapesFn() history2.RestoreShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.Restore(ids)
	}
}

func (m *CanvasManager) newDeleteShapesFn() history2.DeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.GetCanvas().DeleteShapes(ids)
	}
}
