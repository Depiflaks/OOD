package view

import (
	"fmt"
	"image"
	"image/color"

	"gioui.org/app"
	"gioui.org/io/event"
	"gioui.org/io/key"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/widget/material"

	"vector-editor/src/core/modelview"
)

type (
	C = layout.Context
	D = layout.Dimensions
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

	toolbar ToolbarView
	canvas  CanvasView

	locked bool
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

	wv.toolbar = NewToolbarView(wv.window, mv.Toolbar(), fileActions, wv)
	//w.canvas = NewCanvasView(mv.Canvas())

	return wv
}

func (v *workspaceView) Run() error {
	th := material.NewTheme()
	th.Bg = color.NRGBA{R: 255, A: 255}
	var ops op.Ops

	for {
		switch e := v.window.Event().(type) {
		case app.DestroyEvent:
			return e.Err
		case app.FrameEvent:
			gtx := app.NewContext(&ops, e)

			layout.Stack{}.Layout(gtx,
				layout.Expanded(func(gtx layout.Context) layout.Dimensions {
					return layout.Flex{Axis: layout.Vertical}.Layout(gtx,
						layout.Rigid(func(gtx layout.Context) layout.Dimensions {
							return v.toolbar.Process(gtx)
						}),
						layout.Flexed(1, func(gtx layout.Context) layout.Dimensions {
							// TODO: нарисовать canvas
							return layout.Dimensions{Size: image.Point{}}
						}),
					)
				}),
			)

			e.Frame(gtx.Ops)
		}
	}
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

type Locker interface {
	Lock()
	Unlock()
}

func (v *workspaceView) Lock() {
	v.locked = true
}

func (v *workspaceView) Unlock() {
	v.locked = false
}
