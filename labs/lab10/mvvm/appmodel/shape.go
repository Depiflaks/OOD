package appmodel

import (
	"vector-editor/graphics"
	"vector-editor/history"
)

type ShapeManager struct {
	history  *history.History
	selected map[ShapeId]*EditableShape
}

func NewShapeManager(history *history.History) *ShapeManager {
	return &ShapeManager{
		history:  history,
		selected: make(map[ShapeId]*EditableShape),
	}
}

func (m *ShapeManager) AppendToSelection(s *EditableShape, withCtrl bool) {
	if !withCtrl {
		m.ClearSelection()
	}
	sh := (*s).GetShape()
	id := ShapeId(sh.GetShapeId())

	m.selected[id] = s
}

func (m *ShapeManager) IsSelected(s *EditableShape) bool {
	sh := (*s).GetShape()
	id := ShapeId(sh.GetShapeId())
	_, ok := m.selected[id]
	return ok
}

func (m *ShapeManager) GetSelectedShapeIds() []ShapeId {
	ids := make([]ShapeId, 0, len(m.selected))
	for id := range m.selected {
		ids = append(ids, id)
	}
	return ids
}

func (m *ShapeManager) Drag(delta graphics.Vector, isDragging bool) {
	if isDragging {
		cmd := history.NewMoveShapesCommand(
			m.newMoveShapesFn(), delta)
		m.history.AppendAndExecute(cmd)
	}
	for _, s := range m.selected {
		(*s).Move(delta)
	}
}

func (m *ShapeManager) Resize(delta graphics.Vector, bounds graphics.Bounds) {
	for _, s := range m.selected {
		(*s).Scale(delta, bounds)
	}
}

func (m *ShapeManager) ClearSelection() {
	m.selected = make(map[ShapeId]*EditableShape)
}

func (m *ShapeManager) newMoveShapesFn() history.MoveShapesFn {
	return func(delta graphics.Vector) {
		for _, s := range m.selected {
			(*s).GetShape().Move(delta)
		}
	}
}

func (m *ShapeManager) newResizeShapesFn() history.ResizeShapesFn {
	return func(delta graphics.Vector, bounds graphics.Bounds) {
		for _, s := range m.selected {
			(*s).GetShape().UpdateRect(delta, bounds)
		}
	}
}
