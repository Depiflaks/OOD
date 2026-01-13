package manager

import (
	"image/color"
	"vector-editor/src/draw"
	"vector-editor/src/history"
	"vector-editor/src/types"
)

type CanvasManager interface {
	History() history.History
	ShapeManager() ShapeManager

	RegisterCanvas(canvas EditableCanvas)

	NewShape(t types.ShapeType, style draw.Style)
	Delete()
	ClearSelection()
	SetStyle(style draw.Style)
}

type canvasManager struct {
	history      history.History
	canvas       EditableCanvas
	shapeManager privateShapeManager
}

func NewCanvasManager(h history.History) CanvasManager {
	return &canvasManager{
		history:      h,
		shapeManager: NewShapeManager(h),
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

func (m *canvasManager) NewShape(t types.ShapeType, style draw.Style) {
	cmd := history.NewNewShapeCommand(
		m.newCreateShapeFn(t, style),
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

func (m *canvasManager) SetStyle(newStyle draw.Style) {
	if len(m.shapeManager.GetSelectedShapeIds()) == 0 && newStyle.Fill != nil {
		cmd := history.NewSetBackgroundCommand(
			m.newSetBackgroundColorFn(),
			m.canvas.GetCanvas().GetBackground(),
			*newStyle.Fill,
		)
		m.history.AppendAndExecute(cmd)
		return
	}

	if newStyle.BackgroundImagePath != nil && len(m.shapeManager.GetSelectedShapeIds()) != 1 {
		return
	}

	prevStyles := make(map[types.ShapeId]draw.Style)
	newStyles := make(map[types.ShapeId]draw.Style)
	for _, id := range m.shapeManager.GetSelectedShapeIds() {
		s := m.canvas.GetCanvas().GetShape(id)
		prevStyles[id] = s.GetStyle()
		newStyles[id] = newStyle
	}
	cmd := history.NewSetStyleCommand(m.newSetStyleFn(), prevStyles, newStyles)
	m.history.AppendAndExecute(cmd)
}

func (m *canvasManager) newCreateShapeFn(t types.ShapeType, style draw.Style) history.CreateShapeFn {
	return func() types.ShapeId {
		return m.canvas.GetCanvas().NewShape(t, style)
	}
}

func (m *canvasManager) newMarkDeleteShapesFn() history.MarkDeleteShapesFn {
	return func(ids []types.ShapeId) {
		m.canvas.MarkDeleted(ids)
	}
}

func (m *canvasManager) newRestoreShapesFn() history.RestoreShapesFn {
	selected := snapshotSelected(m.shapeManager.GetSelectedShapes())
	return func(ids []types.ShapeId) {
		m.canvas.Restore(ids)
		m.shapeManager.SetSelectedShapes(selected)
	}
}

func (m *canvasManager) newDeleteShapesFn() history.DeleteShapesFn {
	return func(ids []types.ShapeId) {
		m.canvas.GetCanvas().DeleteShapes(ids)
	}
}

func (m *canvasManager) newSetBackgroundColorFn() history.SetBackgroundColorFn {
	return func(color color.Color) {
		m.canvas.GetCanvas().SetBackground(color)
	}
}

func (m *canvasManager) newSetStyleFn() history.SetStyleFn {
	selected := snapshotSelected(m.shapeManager.GetSelectedShapes())
	return func(styles map[types.ShapeId]draw.Style) {
		for id, s := range selected {
			st, ok := styles[id]
			if !ok {
				continue
			}
			s.GetShape().SetStyle(st)
		}
		m.shapeManager.SetSelectedShapes(selected)
	}
}
