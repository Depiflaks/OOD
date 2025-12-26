package appmodel

import (
	"vector-editor/src/geometry"
	"vector-editor/src/history"
	"vector-editor/src/model"
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
		newRects := map[model.ShapeId]geometry.Rect{}
		curRects := map[model.ShapeId]geometry.Rect{}
		for id, s := range m.selected {
			curPos := s.GetShape().GetPosition()
			curBounds := s.GetShape().GetBounds()
			newPos, newBounds := geometry.CalculateScale(
				delta,
				scale,
				curPos,
				curBounds,
			)
			newRects[id] = geometry.Rect{
				Position: newPos,
				Size:     newBounds,
			}
			curRects[id] = geometry.Rect{
				Position: curPos,
				Size:     curBounds,
			}
		}
		cmd := history.NewResizeShapesCommand(
			m.newResizeShapesFn(), newRects, curRects,
		)
		m.history.AppendAndExecute(cmd)
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
	return func(rects map[model.ShapeId]geometry.Rect) {
		for id, s := range m.selected {
			s.GetShape().UpdateRect(rects[id].Position, rects[id].Size)
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
