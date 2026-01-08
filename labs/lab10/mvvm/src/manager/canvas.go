package manager

import (
	"image/color"
	"vector-editor/src/geometry"
	"vector-editor/src/history"
	"vector-editor/src/model"
)

type CanvasManager interface {
	History() history.History
	ShapeManager() ShapeManager

	RegisterCanvas(canvas EditableCanvas)

	NewShape(t model.ShapeType, style geometry.Style)
	Delete()
	ClearSelection()
	SetStyle(style geometry.Style)
}

type canvasManager struct {
	history      history.History
	canvas       EditableCanvas
	shapeManager ShapeManager
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

func (m *canvasManager) NewShape(t model.ShapeType, style geometry.Style) {
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

func (m *canvasManager) SetStyle(newStyle geometry.Style) {
	if len(m.shapeManager.GetSelectedShapeIds()) == 0 && newStyle.Fill != nil {
		cmd := history.NewSetBackgroundCommand(
			m.newSetBackgroundColorFn(),
			m.canvas.GetCanvas().GetBackground(),
			*newStyle.Fill,
		)
		m.history.AppendAndExecute(cmd)
		return
	}

	if newStyle.Image != nil && len(m.shapeManager.GetSelectedShapeIds()) != 1 {
		return
	}

	prevStyles := make(map[model.ShapeId]geometry.Style)
	newStyles := make(map[model.ShapeId]geometry.Style)
	for _, id := range m.shapeManager.GetSelectedShapeIds() {
		s := m.canvas.GetCanvas().GetShape(id)
		prevStyles[id] = s.GetStyle()
		newStyles[id] = newStyle
	}
	cmd := history.NewSetStyleCommand(m.newSetStyleFn(), prevStyles, newStyles)
	m.history.AppendAndExecute(cmd)
	// TODO: подумать над тем, как оповещать изменение selection'
}

func (m *canvasManager) newCreateShapeFn(t model.ShapeType, style geometry.Style) history.CreateShapeFn {
	return func() model.ShapeId {
		return m.canvas.GetCanvas().NewShape(t, style)
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

func (m *canvasManager) newSetBackgroundColorFn() history.SetBackgroundColorFn {
	return func(color color.Color) {
		m.canvas.GetCanvas().SetBackground(color)
	}
}

func (m *canvasManager) newSetStyleFn() history.SetStyleFn {
	ids := m.shapeManager.GetSelectedShapeIds()
	dst := make(map[model.ShapeId]model.Shape, len(ids))
	for _, id := range ids {
		dst[id] = m.canvas.GetCanvas().GetShape(id)
	}
	return func(styles map[model.ShapeId]geometry.Style) {
		for id, s := range dst {
			st, ok := styles[id]
			if !ok {
				continue
			}
			s.SetStyle(st)
		}
	}
}
