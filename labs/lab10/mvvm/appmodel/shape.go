package appmodel

import "vector-editor/graphics"

type ShapeManager struct {
	selected map[ShapeId]*EditableShape
}

func NewShapeManager() *ShapeManager {
	return &ShapeManager{
		selected: make(map[ShapeId]*EditableShape),
	}
}

func (m *ShapeManager) AppendToSelection(s *EditableShape, withCtrl bool) {
	if !withCtrl {
		m.ClearSelection()
	}
	m.selected[(*s).GetShapeId()] = s
}

func (m *ShapeManager) IsSelected(s EditableShape) bool {
	_, ok := m.selected[s.GetShapeId()]
	return ok
}

func (m *ShapeManager) GetSelectedShapeIds() []ShapeId {
	ids := make([]ShapeId, 0, len(m.selected))
	for id := range m.selected {
		ids = append(ids, id)
	}
	return ids
}

func (m *ShapeManager) Drag(delta graphics.Vector) {
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
