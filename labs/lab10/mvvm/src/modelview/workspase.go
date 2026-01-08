package modelview

import (
	"vector-editor/src/manager"
	"vector-editor/src/model"
)

type WorkspaceModelView interface {
	Canvas() CanvasModelView
	Toolbar() *ToolbarModelView

	Undo()
	Redo()

	AddObserver(o WorkspaceModelViewObserver)
}

type workspaceModelView struct {
	workspaceManager manager.WorkspaceManager
	canvas           CanvasModelView
	toolbar          *ToolbarModelView
	observers        []WorkspaceModelViewObserver
}

func NewWorkspaceModelView(
	workspaceManager manager.WorkspaceManager,
	canvas *model.Canvas,
) WorkspaceModelView {
	return &workspaceModelView{
		workspaceManager: workspaceManager,
		canvas:           NewCanvasModelView(canvas, workspaceManager.CanvasManager()),
		toolbar:          NewToolbarModelView(workspaceManager.CanvasManager()),
	}
}

func (w *workspaceModelView) Canvas() CanvasModelView {
	return w.canvas
}

func (w *workspaceModelView) Toolbar() *ToolbarModelView {
	return w.toolbar
}

func (w *workspaceModelView) AddObserver(o WorkspaceModelViewObserver) {
	w.observers = append(w.observers, o)
}

func (w *workspaceModelView) Undo() {
	w.workspaceManager.Undo()
}

func (w *workspaceModelView) Redo() {
	w.workspaceManager.Redo()
}
