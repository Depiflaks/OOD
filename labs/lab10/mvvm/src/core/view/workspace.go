package view

import (
	"fmt"
	"vector-editor/src/core/modelview"

	"gioui.org/app"
	"gioui.org/io/event"
	"gioui.org/io/key"
	"gioui.org/layout"
)

const (
	WindowWidth  = 1024
	WindowHeight = 768
	WindowTitle  = "Vertical Editor"
)

type WorkspaceView interface {
	Run() error
}

type workspaceView struct {
	workspaceMV modelview.WorkspaceModelView

	window *app.Window

	//toolbar ToolbarView
	//canvas  CanvasView
}

func NewWorkspaceView(
	mv modelview.WorkspaceModelView,
	window *app.Window,
) WorkspaceView {
	window.Option(app.Title(WindowTitle), app.Size(WindowWidth, WindowHeight))
	wv := &workspaceView{
		workspaceMV: mv,
		window:      window,
	}

	_ = FileActions{
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

	//w.toolbar = NewToolbarView(w.window, mv.Toolbar(), fileActions)
	//w.canvas = NewCanvasView(mv.Canvas())

	return wv
}

func (v *workspaceView) Run() error {

}

func ProcessKeys(gtx layout.Context, tag event.Tag) {
	for {
		ev, ok := gtx.Event(key.Filter{Focus: tag})
		if !ok {
			break
		}

		if e, ok := ev.(key.Event); ok {
			if e.State == key.Press {
				switch e.Name {
				case key.NameReturn, key.NameEnter:
					fmt.Println("Нажат Enter")
				case key.NameEscape:
					fmt.Println("Нажат Escape")
				case key.NameLeftArrow:
					fmt.Println("Стрелка Влево")
				case key.NameRightArrow:
					fmt.Println("Стрелка Вправо")
				case "Q", "q":
					fmt.Println("Нажата буква Q")
				default:
					fmt.Printf("Нажата клавиша: %v\n", e.Name)
				}
			}
		}
	}
}

//func (v *workspaceView) installShortcuts() {
//	v.window.Canvas().SetOnTypedKey(func(k *fyne.KeyEvent) {
//		switch k.Name {
//		case fyne.KeyZ:
//			v.workspaceMV.Undo()
//		case fyne.KeyU:
//			v.workspaceMV.Redo()
//		case fyne.KeyDelete:
//			v.workspaceMV.Delete()
//		}
//	})
//}
