package modelview

type WorkspaceModelView struct {
	canvas    *CanvasModelView
	toolbar   *ToolbarModelView
	observers []WorkspaceModelViewObserver
}

func NewWorkspaceModelView(
	canvas *CanvasModelView,
	toolbar *ToolbarModelView,
) *WorkspaceModelView {
	return &WorkspaceModelView{
		canvas:  canvas,
		toolbar: toolbar,
	}
}

func (w *WorkspaceModelView) Canvas() *CanvasModelView {
	return w.canvas
}

func (w *WorkspaceModelView) Toolbar() *ToolbarModelView {
	return w.toolbar
}

func (w *WorkspaceModelView) AddObserver(o WorkspaceModelViewObserver) {
	w.observers = append(w.observers, o)
}
