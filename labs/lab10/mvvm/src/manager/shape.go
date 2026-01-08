package manager

import (
	"vector-editor/src/geometry"
	"vector-editor/src/history"
	"vector-editor/src/model"
)

type ShapeManager interface {
	AppendToSelection(s EditableShape, withCtrl bool)
	IsSelected(s EditableShape) bool
	GetSelectedShapeIds() []model.ShapeId
	ClearSelection()

	Drag(delta geometry.Vector, isDragging bool)
	Resize(delta geometry.Vector, scale geometry.Scale, isResizing bool)

	StartDragging()
	StopDragging()
	StartResizing()
	StopResizing()
}

type shapeManager struct {
	history  history.History
	selected map[model.ShapeId]EditableShape
}

func NewShapeManager(history history.History) ShapeManager {
	return &shapeManager{
		history:  history,
		selected: make(map[model.ShapeId]EditableShape),
	}
}

func (m *shapeManager) AppendToSelection(s EditableShape, withCtrl bool) {
	if !withCtrl {
		m.ClearSelection()
	}
	id := s.GetShape().GetShapeId()

	m.selected[id] = s
}

func (m *shapeManager) IsSelected(s EditableShape) bool {
	id := s.GetShape().GetShapeId()
	_, ok := m.selected[id]
	return ok
}

func (m *shapeManager) GetSelectedShapeIds() []model.ShapeId {
	ids := make([]model.ShapeId, 0, len(m.selected))
	for id := range m.selected {
		ids = append(ids, id)
	}
	return ids
}

func (m *shapeManager) Drag(delta geometry.Vector, isDragging bool) {
	if !isDragging {
		cmd := history.NewMoveShapesCommand(
			m.newMoveShapesFn(), delta)
		m.history.AppendAndExecute(cmd)
	}
	for _, s := range m.selected {
		s.Move(delta)
	}
}

func (m *shapeManager) Resize(
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

func (m *shapeManager) StartDragging() {
	for _, s := range m.selected {
		s.StartDragging()
	}
}

func (m *shapeManager) StopDragging() {
	for _, s := range m.selected {
		s.StopDragging()
	}
}

func (m *shapeManager) StartResizing() {
	for _, s := range m.selected {
		s.StartResizing()
	}
}

func (m *shapeManager) StopResizing() {
	for _, s := range m.selected {
		s.StopResizing()
	}
}

func (m *shapeManager) ClearSelection() {
	selected := m.selected

	defer func(sel map[model.ShapeId]EditableShape) {
		for _, s := range sel {
			s.Notify()
		}
	}(selected)

	m.selected = make(map[model.ShapeId]EditableShape)
}

func snapshotSelected(src map[model.ShapeId]EditableShape) map[model.ShapeId]EditableShape {
	dst := make(map[model.ShapeId]EditableShape, len(src))
	for id, s := range src {
		dst[id] = s
	}
	return dst
}

func (m *shapeManager) newMoveShapesFn() history.MoveShapesFn {
	selected := snapshotSelected(m.selected)
	return func(delta geometry.Vector) {
		for _, s := range selected {
			s.GetShape().Move(delta)
		}
	}
}

func (m *shapeManager) newResizeShapesFn() history.ResizeShapesFn {
	selected := snapshotSelected(m.selected)
	return func(rects map[model.ShapeId]geometry.Rect) {
		for id, s := range selected {
			r, ok := rects[id]
			if !ok {
				continue
			}
			s.GetShape().UpdateRect(r.Position, r.Size)
		}
	}
}
