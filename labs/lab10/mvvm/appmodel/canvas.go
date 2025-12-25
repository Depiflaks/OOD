package appmodel

import (
	"vector-editor/history"
	"vector-editor/model"
)

type CanvasManager struct {
	history      *history.History
	canvas       *EditableCanvas
	shapeManager *ShapeManager
}

func NewCanvasManager(
	h *history.History,
	canvas *EditableCanvas,
) *CanvasManager {
	return &CanvasManager{
		history:      h,
		canvas:       canvas,
		shapeManager: NewShapeManager(),
	}
}

func (m *CanvasManager) NewShape(t model.ShapeType) {
	cmd := NewNewShapeCommand(m.canvas, t)
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
	cmd := NewDeleteShapeCommand(m.canvas, ids)
	m.history.AppendAndExecute(cmd)
}
