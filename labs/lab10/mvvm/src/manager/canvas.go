package manager

import (
	"vector-editor/src/history"
	"vector-editor/src/model"
)

type CanvasManager interface {
	History() history.History
	ShapeManager() ShapeManager

	RegisterCanvas(canvas EditableCanvas)

	NewShape(t model.ShapeType)
	Delete()
	ClearSelection()
}

type canvasManager struct {
	history      history.History
	canvas       EditableCanvas
	shapeManager ShapeManager
}

func NewCanvasManager() CanvasManager {
	h := history.NewHistory(10)
	return &canvasManager{
		history:      h,
		shapeManager: NewShapeManager(&h),
	}
}

func (m *canvasManager) History() history.History {
	return m.history
}

func (m *canvasManager) RegisterCanvas(canvas EditableCanvas) {
	m.canvas = canvas
}

func (m *canvasManager) ShapeManager() ShapeManager {
	return m.shapeManager
}

func (m *canvasManager) NewShape(t model.ShapeType) {
	cmd := history.NewNewShapeCommand(
		m.newCreateShapeFn(t),
		m.newMarkDeleteShapesFn(),
		m.newRestoreShapesFn(),
		m.newDeleteShapesFn(),
	)
	m.history.AppendAndExecute(cmd)
}

func (m *canvasManager) ClearSelection() {
	m.shapeManager.ClearSelection()
}

func (m *canvasManager) Delete() {
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

func (m *canvasManager) newCreateShapeFn(t model.ShapeType) history.CreateShapeFn {
	return func() model.ShapeId {
		return m.canvas.GetCanvas().NewShape(t)
	}
}

func (m *canvasManager) newMarkDeleteShapesFn() history.MarkDeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.MarkDeleted(ids)
	}
}

func (m *canvasManager) newRestoreShapesFn() history.RestoreShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.Restore(ids)
	}
}

func (m *canvasManager) newDeleteShapesFn() history.DeleteShapesFn {
	return func(ids []model.ShapeId) {
		m.canvas.GetCanvas().DeleteShapes(ids)
	}
}
