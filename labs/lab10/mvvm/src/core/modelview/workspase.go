package modelview

import (
	"vector-editor/src/core/manager"
	"vector-editor/src/core/model"
)

type WorkspaceModelView interface {
	Canvas() CanvasModelView
	Toolbar() ToolbarModelView

	Undo()
	Redo()

	Save()
	SaveAs(path string)
	Open(path string)

	Delete()

	AddObserver(o WorkspaceModelViewObserver)
}

type workspaceModelView struct {
	workspace        model.Workspace
	workspaceManager manager.WorkspaceManager
	canvas           CanvasModelView
	toolbar          ToolbarModelView
	observers        []WorkspaceModelViewObserver
}

func NewWorkspaceModelView(
	workspaceManager manager.WorkspaceManager,
	workspace model.Workspace,
) WorkspaceModelView {
	return &workspaceModelView{
		workspaceManager: workspaceManager,
		workspace:        workspace,
		canvas:           NewCanvasModelView(workspace.Canvas(), workspaceManager.CanvasManager()),
		toolbar:          NewToolbarModelView(workspaceManager.CanvasManager()),
	}
}

func (w *workspaceModelView) Delete() {
	w.workspaceManager.Delete()
}

func (w *workspaceModelView) Save() {
	w.workspace.Save()
}

func (w *workspaceModelView) SaveAs(path string) {
	w.workspace.SaveAs(path)
}

func (w *workspaceModelView) Open(path string) {
	w.workspace.Open(path)
}

func (w *workspaceModelView) Canvas() CanvasModelView {
	return w.canvas
}

func (w *workspaceModelView) Toolbar() ToolbarModelView {
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
