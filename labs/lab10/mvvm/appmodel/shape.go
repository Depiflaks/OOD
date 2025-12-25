package appmodel

import (
	"vector-editor/geometry"
	"vector-editor/history"
	"vector-editor/model"
)

type ShapeManager struct {
	history  *history.History
	selected map[model.ShapeId]EditableShape
}

func NewShapeManager(history *history.History) *ShapeManager {
	return &ShapeManager{
		history:  history,
		selected: make(map[model.ShapeId]EditableShape),
	}
}

func (m *ShapeManager) AppendToSelection(s EditableShape, withCtrl bool) {
	if !withCtrl {
		m.ClearSelection()
	}
	id := s.GetShape().GetShapeId()

	m.selected[id] = s
}

func (m *ShapeManager) IsSelected(s EditableShape) bool {
	id := s.GetShape().GetShapeId()
	_, ok := m.selected[id]
	return ok
}

func (m *ShapeManager) GetSelectedShapeIds() []model.ShapeId {
	ids := make([]model.ShapeId, 0, len(m.selected))
	for id := range m.selected {
		ids = append(ids, id)
	}
	return ids
}

func (m *ShapeManager) Drag(delta geometry.Vector, isDragging bool) {
	if !isDragging {
		cmd := history.NewMoveShapesCommand(
			m.newMoveShapesFn(), delta)
		m.history.AppendAndExecute(cmd)
	}
	for _, s := range m.selected {
		s.Move(delta)
	}
}

func (m *ShapeManager) Resize(
	delta geometry.Vector,
	scale geometry.Scale,
	isResizing bool,
) {
	if !isResizing {

	}
	for _, s := range m.selected {
		s.Scale(delta, scale)
	}
}

func (m *ShapeManager) StartDragging() {
	for _, s := range m.selected {
		s.StartDragging()
	}
}

func (m *ShapeManager) StopDragging() {
	for _, s := range m.selected {
		s.StopDragging()
	}
}

func (m *ShapeManager) StartResizing() {
	for _, s := range m.selected {
		s.StartResizing()
	}
}

func (m *ShapeManager) StopResizing() {
	for _, s := range m.selected {
		s.StopResizing()
	}
}

func (m *ShapeManager) ClearSelection() {
	m.selected = make(map[model.ShapeId]EditableShape)
}

func (m *ShapeManager) SetStyle(newStyle geometry.Style) {
	prevStyles := make(map[model.ShapeId]geometry.Style)
	newStyles := make(map[model.ShapeId]geometry.Style)
	for id, s := range m.selected {
		prevStyles[id] = s.GetShape().GetStyle()
		newStyles[id] = newStyle
	}
	cmd := history.NewSetStyleCommand(m.newSetStyleFn(), prevStyles, newStyles)
	m.history.AppendAndExecute(cmd)
}

func (m *ShapeManager) newMoveShapesFn() history.MoveShapesFn {
	return func(delta geometry.Vector) {
		for _, s := range m.selected {
			s.GetShape().Move(delta)
		}
	}
}

func (m *ShapeManager) newResizeShapesFn() history.ResizeShapesFn {
	return func(delta geometry.Vector, bounds geometry.Bounds) {
		for _, s := range m.selected {
			s.GetShape().UpdateRect(delta, bounds)
		}
	}
}

func (m *ShapeManager) newSetStyleFn() history.SetStyleFn {
	return func(styles map[model.ShapeId]geometry.Style) {
		for id, s := range m.selected {
			s.GetShape().SetStyle(styles[id])
		}
	}
}
