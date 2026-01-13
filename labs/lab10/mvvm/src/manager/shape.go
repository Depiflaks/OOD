package manager

import (
	"image/color"
	"vector-editor/src/draw"
	"vector-editor/src/geometry"
	"vector-editor/src/history"
	"vector-editor/src/types"
)

type ShapeManagerObserver interface {
	OnSelectionChange(style draw.Style, count int)
}

type privateShapeManager interface {
	ShapeManager

	GetSelectedShapeIds() []types.ShapeId
	GetSelectedShapes() map[types.ShapeId]EditableShape
	SetSelectedShapes(newSelected map[types.ShapeId]EditableShape)
	ClearSelection()
}

type ShapeManager interface {
	AppendToSelection(s EditableShape, withCtrl bool)
	IsSelected(s EditableShape) bool

	Drag(delta geometry.Vector, isDragging bool)
	Resize(delta geometry.Vector, scale geometry.Scale, isResizing bool)

	StartDragging()
	StopDragging()
	StartResizing()
	StopResizing()

	AddObserver(o ShapeManagerObserver)
}

type shapeManager struct {
	observers []ShapeManagerObserver
	history   history.History
	selected  map[types.ShapeId]EditableShape
}

func NewShapeManager(history history.History) privateShapeManager {
	return &shapeManager{
		history:  history,
		selected: make(map[types.ShapeId]EditableShape),
	}
}

func (m *shapeManager) AddObserver(o ShapeManagerObserver) {
	m.observers = append(m.observers, o)
}

func (m *shapeManager) SetSelectedShapes(newSelected map[types.ShapeId]EditableShape) {
	m.selected = newSelected
	m.notifySelectionChanged()
}

func (m *shapeManager) AppendToSelection(s EditableShape, withCtrl bool) {
	if !withCtrl {
		m.ClearSelection()
	}
	id := s.GetShape().GetShapeId()

	m.selected[id] = s
	m.notifySelectionChanged()
}

func (m *shapeManager) IsSelected(s EditableShape) bool {
	id := s.GetShape().GetShapeId()
	_, ok := m.selected[id]
	return ok
}

func (m *shapeManager) GetSelectedShapeIds() []types.ShapeId {
	ids := make([]types.ShapeId, 0, len(m.selected))
	for id := range m.selected {
		ids = append(ids, id)
	}
	return ids
}

func (m *shapeManager) GetSelectedShapes() map[types.ShapeId]EditableShape {
	return m.selected
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
		newRects := map[types.ShapeId]geometry.Rect{}
		curRects := map[types.ShapeId]geometry.Rect{}
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

	defer func(sel map[types.ShapeId]EditableShape) {
		for _, s := range sel {
			s.Notify()
		}
	}(selected)

	m.selected = make(map[types.ShapeId]EditableShape)
	m.notifySelectionChanged()
}

func snapshotSelected(src map[types.ShapeId]EditableShape) map[types.ShapeId]EditableShape {
	dst := make(map[types.ShapeId]EditableShape, len(src))
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
		m.SetSelectedShapes(selected)
	}
}

func (m *shapeManager) newResizeShapesFn() history.ResizeShapesFn {
	selected := snapshotSelected(m.selected)
	return func(rects map[types.ShapeId]geometry.Rect) {
		for id, s := range selected {
			r, ok := rects[id]
			if !ok {
				continue
			}
			s.GetShape().UpdateRect(r.Position, r.Size)
		}
		m.SetSelectedShapes(selected)
	}
}

func (m *shapeManager) notifySelectionChanged() {
	var common draw.Style

	first := true
	var fill *color.Color
	var stroke *color.Color

	eqColorPtr := func(a, b *color.Color) bool {
		if a == nil && b == nil {
			return true
		}
		if a == nil || b == nil {
			return false
		}
		if *a == nil && *b == nil {
			return true
		}
		if *a == nil || *b == nil {
			return false
		}
		ra, ga, ba, aa := (*a).RGBA()
		rb, gb, bb, ab := (*b).RGBA()
		return ra == rb && ga == gb && ba == bb && aa == ab
	}

	for _, es := range m.selected {
		st := es.GetShape().GetStyle()

		if first {
			fill = st.Fill
			stroke = st.Stroke
			first = false
			continue
		}

		if !eqColorPtr(fill, st.Fill) {
			fill = nil
		}
		if !eqColorPtr(stroke, st.Stroke) {
			stroke = nil
		}
	}

	common.Fill = fill
	common.Stroke = stroke

	for _, o := range m.observers {
		o.OnSelectionChange(common, len(m.selected))
	}
}
