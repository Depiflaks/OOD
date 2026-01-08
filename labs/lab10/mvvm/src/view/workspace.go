package view

import (
	"vector-editor/src/modelview"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
)

type WorkspaceView interface {
	Show()
}

type workspaceView struct {
	workspaceMV modelview.WorkspaceModelView

	app    fyne.App
	window fyne.Window

	toolbar ToolbarView
	canvas  CanvasView

	files  FileActions
	colors ColorActions
}

func NewWorkspaceView(
	a fyne.App,
	mv modelview.WorkspaceModelView,
	files FileActions,
	colors ColorActions,
) WorkspaceView {
	w := &workspaceView{
		app:         a,
		window:      a.NewWindow("Vector Editor"),
		files:       files,
		colors:      colors,
		workspaceMV: mv,
	}

	w.toolbar = NewToolbarView(w.window, mv.Toolbar(), files, colors)
	w.canvas = NewCanvasView(mv.Canvas())

	content := container.NewBorder(w.toolbar.Object(), nil, nil, nil, w.canvas)
	w.window.SetContent(content)
	w.window.Resize(fyne.NewSize(1024, 768))

	w.installShortcuts()

	return w
}

func (w *workspaceView) Show() {
	w.window.Show()
}

func (w *workspaceView) installShortcuts() {
	w.window.Canvas().SetOnTypedKey(func(k *fyne.KeyEvent) {
		switch k.Name {
		case fyne.KeyZ:
			w.workspaceMV.Undo()
		case fyne.KeyU:
			w.workspaceMV.Redo()
		}
	})
}
