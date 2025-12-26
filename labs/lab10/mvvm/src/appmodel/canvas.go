package appmodel

import (
	"vector-editor/src/history"
	"vector-editor/src/model"
)

type CanvasManager struct {
	history      history.History
	canvas       EditableCanvas
	shapeManager *ShapeManager
}

func NewCanvasManager(
	h history.History,
	canvas EditableCanvas,
) *CanvasManager {
	return &CanvasManager{
		history:      h,
		canvas:       canvas,
		shapeManager: NewShapeManager(&h),
	}
}

func (m *CanvasManager) ShapeManager() *ShapeManager {
	return m.shapeManager
}

func (m *CanvasManager) NewShape(t model.ShapeType) {
	cmd := history.NewNewShapeCommand(
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
	cmd := history.NewDeleteShapeCommand(
		ids,
		m.newMarkDeleteShapesFn(),
		m.newRestoreShapesFn(),
		m.newDeleteShapesFn(),
	)
	m.history.AppendAndExecute(cmd)
}

func (m *CanvasManager) newCreateShapeFn(t model.ShapeType) history.CreateShapeFn {
	return func() model.ShapeId {
		return m.canvas.GetCanvas().NewShape(t)
	}
}

func (m *CanvasManager) newMarkDeleteShapesFn() history.MarkDeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.MarkDeleted(ids)
	}
}

func (m *CanvasManager) newRestoreShapesFn() history.RestoreShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.Restore(ids)
	}
}

func (m *CanvasManager) newDeleteShapesFn() history.DeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.GetCanvas().DeleteShapes(ids)
	}
}
