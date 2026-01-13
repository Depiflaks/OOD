package manager

import (
	"vector-editor/src/history"
)

type WorkspaceManager interface {
	CanvasManager() CanvasManager

	Delete()

	Undo()
	Redo()
}

type workspaceManager struct {
	history history.History
	canvas  CanvasManager
}

func NewWorkspaceManager() WorkspaceManager {
	h := history.NewHistory(10)
	return &workspaceManager{
		history: h,
		canvas:  NewCanvasManager(h),
	}
}

func (w workspaceManager) Delete() {
	w.canvas.Delete()
}

func (w workspaceManager) CanvasManager() CanvasManager {
	return w.canvas
}

func (w workspaceManager) Undo() {
	w.history.Undo()
}

func (w workspaceManager) Redo() {
	w.history.Redo()
}
