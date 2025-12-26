package view

import (
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
)

type WorkspaceView struct {
	app    fyne.App
	window fyne.Window

	toolbar *ToolbarView
	canvas  *CanvasView

	files  FileActions
	colors ColorActions
}

func NewWorkspaceView(
	a fyne.App,
	mv *modelview.WorkspaceModelView,
	files FileActions,
	colors ColorActions,
) *WorkspaceView {
	w := &WorkspaceView{
		app:    a,
		window: a.NewWindow("Vector Editor"),
		files:  files,
		colors: colors,
	}

	w.toolbar = NewToolbarView(w.window, mv.Toolbar(), files, colors)
	w.canvas = NewCanvasView(mv.Canvas())

	content := container.NewBorder(w.toolbar.Object(), nil, nil, nil, w.canvas)
	w.window.SetContent(content)
	w.window.Resize(fyne.NewSize(1024, 768))

	return w
}

func (w *WorkspaceView) Show() {
	w.window.Show()
}
