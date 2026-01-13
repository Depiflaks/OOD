package view

import (
	"log"
	"os"
	"vector-editor/src/core/modelview"

	"gioui.org/app"
)

type WorkspaceView interface {
	Show()
}

type workspaceView struct {
	workspaceMV modelview.WorkspaceModelView

	toolbar ToolbarView
	canvas  CanvasView
}

func NewWorkspaceView(
	mv modelview.WorkspaceModelView,
) WorkspaceView {
	go func() {
		w := new(app.Window)
		w.Option(app.Title("Vector Editor"), app.Size(400, 400))
		if err := loop(w); err != nil {
			log.Fatal(err)
		}
		os.Exit(0)
	}()
	app.Main()
	w := &workspaceView{
		app:         a,
		window:      a.NewWindow("Vector Editor"),
		workspaceMV: mv,
	}

	fileActions := FileActions{
		Open: func(path string) {
			mv.Open(path)
		},
		Save: func() {
			mv.Save()
		},
		SaveAs: func(path string) {
			mv.SaveAs(path)
		},
	}

	w.toolbar = NewToolbarView(w.window, mv.Toolbar(), fileActions)
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
		case fyne.KeyDelete:
			w.workspaceMV.Delete()
		}
	})
}
